#!/usr/bin/env python3

import sys
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
                             QTextEdit, QPushButton, QLineEdit, QFileDialog, QLabel, QStatusBar)
from PyQt5.QtCore import QThread, pyqtSignal, QTimer, QCoreApplication
from PyQt5.QtGui import QImage, QPixmap
import serial
import serial.tools.list_ports
from PIL import Image
import os

class SerialThread(QThread):
    data_received = pyqtSignal(bytes)

    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial_connection = None
        self.running = False

    def run(self):
        try:
            self.serial_connection = serial.Serial(self.port, self.baudrate, timeout=1)
            self.running = True
            while self.running:
                if self.serial_connection.in_waiting:
                    data = self.serial_connection.read(self.serial_connection.in_waiting)
                    self.data_received.emit(data)
                time.sleep(0.01)  # Add a short sleep interval to reduce CPU usage
        except serial.SerialException as e:
            print(f"Serial error: {e}")

    def stop(self):
        self.running = False
        if self.serial_connection:
            self.serial_connection.close()

class UARTGUI(QMainWindow):
    IMAGE_START = b'--IMAGE_START--'
    IMAGE_END = b'--IMAGE_END--'
    TEXT_DELIMITER = b'\n'  # Assuming '\n' as end of text message

    def __init__(self):
        super().__init__()
        self.serial_send = None
        self.serial_receive = None
        self.received_buffer = bytearray()
        self.receiving_image = False

        self.initUI()

    def initUI(self):
        self.setWindowTitle('UART Communication')

        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        layout = QVBoxLayout()

        port_layout = QHBoxLayout()
        self.send_port_label = QLabel('Send:')
        self.send_port_entry = QLineEdit('/dev/ttyACM0')
        self.send_port_entry.textChanged.connect(self.update_ports)
        self.receive_port_label = QLabel('Receive:')
        self.receive_port_entry = QLineEdit('/dev/ttyACM1')
        self.receive_port_entry.textChanged.connect(self.update_ports)
        port_layout.addWidget(self.send_port_label)
        port_layout.addWidget(self.send_port_entry)
        port_layout.addWidget(self.receive_port_label)
        port_layout.addWidget(self.receive_port_entry)
        layout.addLayout(port_layout)

        self.text_to_send = QTextEdit()
        self.text_to_send.setPlaceholderText("Text to send")
        layout.addWidget(self.text_to_send)

        self.received_text = QTextEdit()
        self.received_text.setReadOnly(True)
        self.received_text.setPlaceholderText("Received text (or image)")
        layout.addWidget(self.received_text)

        button_layout = QHBoxLayout()
        self.pick_image_button = QPushButton('Pick Image')
        self.pick_image_button.clicked.connect(self.pick_image)
        self.send_button = QPushButton('Send')
        self.send_button.clicked.connect(self.send_data)
        button_layout.addWidget(self.pick_image_button)
        button_layout.addWidget(self.send_button)
        layout.addLayout(button_layout)

        central_widget.setLayout(layout)

        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)

        self.start_receiving()

    def update_status(self, message):
        self.status_bar.showMessage(message)
        QCoreApplication.processEvents()  # Ensure the GUI updates

    def pick_image(self):
        options = QFileDialog.Options()
        file_filter = "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.tiff *.ppm)"
        file_path, _ = QFileDialog.getOpenFileName(self, "Pick Image File", "", file_filter, options=options)
        if file_path:
            self.send_image(file_path)

    def send_data(self):
        text = self.text_to_send.toPlainText().strip()
        if text and self.serial_send:
            self.update_status("Sending text data...")
            self.serial_send.serial_connection.write(text.encode() + self.TEXT_DELIMITER)
            print(f"Sent text data: {text}")
            self.update_status("Text data sent.")

    def send_image(self, file_path):
        if self.serial_send:
            self.update_status(f"Sending image {os.path.basename(file_path)}...")
            with open(file_path, 'rb') as f:
                data = f.read()
                self.serial_send.serial_connection.write(self.IMAGE_START + data + self.IMAGE_END)
                print(f"Sent file data: {file_path}, size: {len(data)}")
            self.update_status("Image data sent.")

    def update_ports(self):
        if self.serial_send:
            self.serial_send.stop()
        if self.serial_receive:
            self.serial_receive.stop()

        send_port = self.send_port_entry.text()
        receive_port = self.receive_port_entry.text()

        if send_port == receive_port:
            self.serial_send = SerialThread(send_port, 115200)
            self.serial_send.data_received.connect(self.receive_data)
            self.serial_send.start()
            self.serial_receive = self.serial_send
        else:
            self.serial_send = SerialThread(send_port, 115200)
            self.serial_receive = SerialThread(receive_port, 115200)
            self.serial_receive.data_received.connect(self.receive_data)
            self.serial_send.start()
            self.serial_receive.start()

        print(f"Updated ports: Send - {self.send_port_entry.text()}, Receive - {self.receive_port_entry.text()}")
        self.update_status("Ports updated.")

    def receive_data(self, data):
        self.update_status("Receiving data...")
        self.received_buffer.extend(data)

        while self.received_buffer:
            if self.receiving_image:
                image_end_index = self.received_buffer.find(self.IMAGE_END)
                if image_end_index != -1:
                    image_buffer = self.received_buffer[:image_end_index]
                    self.display_image(image_buffer)
                    self.received_buffer = self.received_buffer[image_end_index + len(self.IMAGE_END):]
                    self.receiving_image = False
                else:
                    break
            else:
                image_start_index = self.received_buffer.find(self.IMAGE_START)
                if image_start_index != -1:
                    if image_start_index > 0:
                        self.process_received_text(self.received_buffer[:image_start_index].decode())
                    self.received_buffer = self.received_buffer[image_start_index + len(self.IMAGE_START):]
                    self.receiving_image = True
                else:
                    if self.TEXT_DELIMITER in self.received_buffer:
                        parts = self.received_buffer.split(self.TEXT_DELIMITER, 1)
                        self.process_received_text(parts[0].decode())
                        self.received_buffer = parts[1]
                    else:
                        break
        self.update_status("Data received.")

    def process_received_text(self, text):
        if text:
            self.received_text.append(text)
            print(f"Processed text data: {text}")

    def display_image(self, data):
        with open("received_image.png", "wb") as f:
            f.write(data)
        image = QImage("received_image.png")
        pixmap = QPixmap.fromImage(image)
        self.received_text.append('\n[Image received]')
        self.received_text.append('')
        cursor = self.received_text.textCursor()
        cursor.insertImage(image)
        print(f"Displayed image of size: {len(data)}")

    def start_receiving(self):
        self.update_ports()

    def closeEvent(self, event):
        if self.serial_send:
            self.serial_send.stop()
        if self.serial_receive:
            self.serial_receive.stop()
        event.accept()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = UARTGUI()
    ex.show()
    sys.exit(app.exec_())
