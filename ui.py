# User interface for ES50 Fall 2016 Team 10: Customizable LED Jacket
# Authors: Hailey James, Melissa ChaeHyun Lee, Annie Lin, Stephen Slater

from Tkinter import *
import serial
import pyperclip

serial_speed = 115200
serial_port = '/dev/cu.usbmodem1411'

# Create & configure root
root = Tk()
Grid.rowconfigure(root, 0, weight=1)
Grid.columnconfigure(root, 0, weight=1)

# Create & configure frame
frame=Frame(root)
frame.grid(row=0, column=0, sticky=N+S+E+W)

# Global variables
matrix = [[0 for i in range(13)] for j in range(13)]
buttons = []

class MyButton:
    def __init__(self, x, y):
        self.btn =  Button(frame, text = "X", command=lambda row=x, column=y: self.toggle_text(row, column))
        self.xval = x
        self.yval = y

    def toggle_text(self, row, column):
        """toggle button text between R and G and B"""
        if self.btn["text"] == "X":
            self.btn["text"] = "R"
            self.btn["highlightbackground"] ="red"
            matrix[row][column] = 1
        elif self.btn["text"] == "R":
            self.btn["text"] = "O"
            self.btn["highlightbackground"] ="orange"
            matrix[row][column] = 2
        elif self.btn["text"] == "O":
            self.btn["text"] = "Y"
            self.btn["highlightbackground"] ="yellow"
            matrix[row][column] = 3
        elif self.btn["text"] == "Y":
            self.btn["text"] = "G"
            self.btn["highlightbackground"] ="green"
            matrix[row][column] = 4
        elif self.btn["text"] == "G":
            self.btn["text"] = "B"
            self.btn["highlightbackground"] ="blue"
            matrix[row][column] = 5
        elif self.btn["text"] == "B":
            self.btn["text"] = "P"
            self.btn["highlightbackground"] ="purple"
            matrix[row][column] = 6
        elif self.btn["text"] == "P":
            self.btn["text"] = "X"
            self.btn["highlightbackground"] ="white"
            matrix[row][column] = 0

def sendInfo():
    print matrix
    string = ''
    new_strings = []
    for i in range(13):
        for j in range(13):
            string += str(matrix[i][j])
    for i in range(13):
        if i == 0:
            new_strings.append(string[13*i+12:13*i:-1])
            new_strings.append(string[0])
        elif i%2 == 0:
            new_strings.append(string[13*i+12:13*i-1:-1])
        else:
            new_strings.append(string[13*i:13*i+13])
    string = "".join(new_strings)
    print string
    new_string = ''
    new_string = new_string + '0' + string[0: 60] + '\n'
    new_string = new_string + '1' + string[60: 120] + '\n'
    if (flashButt.btn["text"]=="Flash ON"):
        new_string = new_string + '2' + string[120:] + '1'
    else:
        new_string = new_string + '2' + string[120:] + '0'
    print new_string
    # try:
    #     print "Trying via USB"
    #     ser = serial.Serial(serial_port, serial_speed, timeout=1)
    #     ser.write(string)
    #     ser.close()
    #     print "Done USB"
    # except: 
    print "CRY, just paste the string to the app"
    pyperclip.copy(new_string)

def resetButtons():
    for i in range(13):
        for j in range(13):
            if matrix[i][j] == 0:
                buttons[i*13+j].btn["text"] = "X"
                buttons[i*13+j].btn["highlightbackground"] ="white"
            elif matrix[i][j] == 1:
                buttons[i*13+j].btn["text"] = "R"
                buttons[i*13+j].btn["highlightbackground"] ="red"
            elif matrix[i][j] == 2:
                buttons[i*13+j].btn["text"] = "O"
                buttons[i*13+j].btn["highlightbackground"] ="orange"
            elif matrix[i][j] == 3:
                buttons[i*13+j].btn["text"] = "Y"
                buttons[i*13+j].btn["highlightbackground"] ="yellow"
            elif matrix[i][j] == 4:
                buttons[i*13+j].btn["text"] = "G"
                buttons[i*13+j].btn["highlightbackground"] ="green"
            elif matrix[i][j] == 5:
                buttons[i*13+j].btn["text"] = "B"
                buttons[i*13+j].btn["highlightbackground"] ="blue"
            elif matrix[i][j] == 6:
                buttons[i*13+j].btn["text"] = "P"
                buttons[i*13+j].btn["highlightbackground"] ="purple"

class sendButton:
    def __init__(self):
        self.btn = Button(frame, text = "SEND",command=sendInfo)

class flashButton:
    def __init__(self):
        self.btn =  Button(frame, text = "Flash OFF", command=self.toggle_flash)

    def toggle_flash(self):
        """toggle button text between ON and OFF"""
        if self.btn["text"] == "Flash OFF":
            self.btn["text"] = "Flash ON"
        elif self.btn["text"] == "Flash ON":
            self.btn["text"] = "Flash OFF"

class clearButton:
    def __init__(self):
        self.btn = Button(frame, text = "CLEAR",command=self.clear)

    def clear(self):
        # Reset matrix and buttons
        for i in range(13):
            for j in range(13):
                matrix[i][j] = 0
        for button in buttons:
            button.btn["text"] = "X"
            button.btn["highlightbackground"] ="white"
        sendInfo() 

class rainbowButton:
    def __init__(self):
        self.btn = Button(frame, text = "RAINBOW",command=self.sendRainbow)

    def sendRainbow(self):
        # Reset buttons and matrix
        counter = 0
        for i in range(13):
            for j in range(13):
                matrix[i][j] = (counter % 6) + 1
                counter = counter + 1
        resetButtons()
        sendInfo()

class surpriseButton:
    def __init__(self):
        self.btn = Button(frame, text = "SURPRISE",command=self.sendSurprise)

    def sendSurprise(self):
        # ES50 Design
        design = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [5, 5, 5, 0, 5, 5, 0, 5, 5, 0, 5, 5, 5], [5, 0, 0, 0, 5, 0, 0, 5, 0, 0, 5, 0, 5], [5, 5, 0, 0, 5, 0, 0, 5, 5, 0, 5, 0, 5], [5, 0, 0, 0, 0, 5, 0, 0, 5, 0, 5, 0, 5], [5, 0, 0, 0, 0, 5, 0, 0, 5, 0, 5, 0, 5], [5, 5, 5, 0, 5, 5, 0, 5, 5, 0, 5, 5, 5], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]        # Note: Cannot simply reset matrix, has to be done like this
        for i in range(13):
            for j in range(13):
                matrix[i][j] = design[i][j]
        resetButtons()
        sendInfo()

class heartButton:
    def __init__(self):
        self.btn = Button(frame, text = "HEART",command=self.sendHeart)

    def sendHeart(self):
        # Heart Design
        design = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0], [0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0], [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0], [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0], [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0], [0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0], [0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0], [0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0], [0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]
        # Note: Cannot simply reset matrix, has to be done like this
        for i in range(13):
            for j in range(13):
                matrix[i][j] = design[i][j]
        resetButtons()
        sendInfo()

class treeButton:
    def __init__(self):
        self.btn = Button(frame, text = "TREE",command=self.sendTree)

    def sendTree(self):
        # Tree Design
        design = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0], [0, 0, 0, 0, 4, 4, 1, 4, 4, 0, 0, 0, 0], [0, 0, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0], [0, 0, 0, 4, 4, 4, 4, 3, 4, 4, 0, 0, 0], [0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0], [0, 0, 0, 4, 1, 4, 4, 4, 4, 4, 0, 0, 0], [0, 0, 4, 4, 4, 4, 4, 4, 1, 4, 4, 0, 0], [0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0], [0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0]]
        # Note: Cannot simply reset matrix, has to be done like this
        for i in range(13):
            for j in range(13):
                matrix[i][j] = design[i][j]
        resetButtons()
        sendInfo()

#Create a 13x13 (rows x columns) grid of buttons inside the frame
for row_index in range(13):
    Grid.rowconfigure(frame, row_index, weight=1)
    for col_index in range(13):
        myB = MyButton(row_index, col_index)
        myB.btn.grid(row=row_index, column=col_index, sticky=N+S+E+W)
        buttons.append(myB)

flashButt = flashButton()
flashButt.btn.grid(row=3, column=14, sticky=N+S+E+W)
sendButt = sendButton()
sendButt.btn.grid(row=4, column=14, sticky=N+S+E+W)
clearButt = clearButton()
clearButt.btn.grid(row=5, column=14, sticky=N+S+E+W)

rainbowButt = rainbowButton()
rainbowButt.btn.grid(row=7, column=14, sticky=N+S+E+W)

surpriseButt = surpriseButton()
surpriseButt.btn.grid(row=8, column=14, sticky=N+S+E+W)

heartButt = heartButton()
heartButt.btn.grid(row=9, column=14, sticky=N+S+E+W)

treeButt = treeButton()
treeButt.btn.grid(row=10, column=14, sticky=N+S+E+W)

root.mainloop()
