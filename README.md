# Image Steganography using LSB Technique

This project hides and extracts secret data within BMP images using the Least Significant Bit (LSB) technique.

## Features
- Encode secret data into an image
- Decode hidden data from an image
- Uses bitwise operations for data embedding
- Accepts input through command-line arguments

## Technologies Used
- C Programming
- File Handling
- Bitwise Operations

## Learning Outcomes
- Understanding of how image data is stored in BMP format
- Bit-level data manipulation using LSB technique
- Practical use of file handling in C

## How to Run

Compile:
make

Encode:
./encode <source_image.bmp> <secret_file> <output_image.bmp>

Decode:
./decode <stego_image.bmp>
