"""
@author Tom Kirby

This module reads characters from the socket and then processes them to the UI
"""

"""
imports:
math, used for vector math
keyboard, used to steal keybaord/controller inputs to send over socket
matplotlib, used to graph the incoming data set to TkAgg configuration
pandas, stat library build on matplotlib to draw out the data
socket, used for connecting to the socket
sleep, used to pauseafter sending commands and wait for return
"""
import math
import keyboard 
import pandas
import matplotlib 
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import socket
from time import sleep
"""
global variables:
command, string slowly built over successive characters to make the individual lines sent from CyBot
distance, total distance the CyBot has moved
startchar, used in parsechar to determine start and end of commands
position, x and y position of the center of the CyBot
objects, listof objects that we have recorded, their distance, and what color they should be drawn as
angle, angle the CyBot is facing (90 is up 270 is down)
"""

command = ""
distance = 0
startchar = ''
position = [0,0]
movenum = -1
objects = [[0,0,0]]
angle = 90
"""
This function takes in a character and processes it
param:
received, the character to be processed

this function determines if we are in the middle of a command then appends the character to the command string
then if we are closing the command it sends it to its respective process function

return:
nothing
"""
def parseChar(received):
    global command
    global startchar
    if startchar == '': #if not in command
        #check if we need to start a command
        if received == '@':
            startchar = '@'
            command += received
        elif received == '%':
            startchar = '%'
            command += received
        elif received == '!':
            startchar = '!'
            command += received
        elif received == '#':
            startchar = '#'
            command += received
        elif received == '^':
            startchar = '^'
            command += received
    else:    
        
        if received == startchar: #if at end of command
            command += received
            tosend = command
            command = ""
            startchar = ''
            insertData(tosend) #insert data
        else:
            command += received #otherwise just append to command string
    
    
    
"""
This function takes in a measurement and converts it to a vector
params:
angle, direction the object is in
distance, how far away the obj is

this uses trig functions to get the x and y compositions of the vector

return:
a 2d vector indication the x and y magnitude of the param
"""
def vectorize(angle,distance):
    ycomp = math.sin(math.radians(angle))*distance
    xcomp = math.cos(math.radians(angle))*distance
    return [xcomp,ycomp]


"""
this function separates and sends the data to the correct function
params:
data, string that represents the command to be sent

this function checks the last character of data and then passes data to its next insert function

"""
def insertData(data):
    if data[:1] == '@':
        print("ping data")
        insertPing(data)
    if data[:1] == '%':
        print("tape data")
        insertTape(data)
    if data[:1] == '!':
        print("pit data")
        insertPit(data)
    if data[:1] == '#':
        print("bump data")
        insertBump(data)
    if data[:1] == '^':
        print("move data")
        insertMove(int(data.replace("^",''))/10.0)#move has a slightly different format due to parsing issues


"""
this function inserts ping data
params:
data, string that represents the command to be inserted

this function parses data and then inserts the ping to the objects array as [xpos,ypos,1]

"""
def insertPing(data):
    global position
    global angle
    global objects
    #prep data for math
    clean = data.replace('@','')
    oldrow = clean.split(",")
    row = [oldrow[1],oldrow[0]] #reverse the input
    row[0] = int(int(row[0])+4) #add 4 cm to account for servo mount point
    finalangle = int(row[1]) + angle - 90 # makes the vector relative to where the robot is facing
    if row[0]<=100: #distances greater than 100 are ignored
        v1 = vectorize(finalangle,row[0]) #converts to2d vector
        #10cm angle = angle
        v2 = vectorize(angle,10) #vector representing center of robot to servo
        xpos = round(v1[0]+v2[0]+position[0],2) #vector addition
        ypos = round(v1[1]+v2[1]+position[1],2)
        vector = [xpos,ypos,1] #final vector
        objects.append(vector) #add to objects
        
"""
this function inserts move data
params:
distance, how far the robot moved

this function takes in the distance moved and angle the robot is facing to change its x and y position

"""
def insertMove(distance):
    global position
    global angle
    global move
    v1 = vectorize(angle,distance)#determines change in x and y positions while moving
    position[0] += v1[0] # update its position
    position[1] += v1[1]
    
    objects.append([position[0],position[1],0])#add move data
    objects[0] = ([position[0],position[1], 0])#first obj is always the robot


"""
this function turnsthe robot
params:
direction, which direction the robot turned left or right

this function takes in the direction turned and then updates the robot's internal angle

"""
def turn(direction):
    global angle
    angle += 45*direction
    if angle > 360:
        angle -= 360
    if angle < 0:
        angle += 360

"""
this function inserts pit data
params:
data, string that represents the command to be inserted

this function parses data and then inserts the pit to the objects array as [xpos,ypos,1]

"""
def insertPit(data):
    global position
    global objects
    clean = data.replace('!','') #cleans data string
    vector = []
    if clean[1] == '1' and clean [2] == '1': #determine which sensor it was from then get the vector of its x and y position
        #front sensor
        vector = vectorize(angle,16)
    elif clean[1] == '1':
        vector = vectorize(angle-10,16)
    elif clean[2] == '1':
        vector = vectorize(angle+10,16)
    elif clean[3] == '1':
        vector = vectorize(angle-60,16)
    elif clean[0] == '1':
        vector = vectorize(angle+60,16)
        
    objects.append([vector[0]+position[0],vector[1]+position[1],1]) #add pit to objects relative to curr pos
    

"""
this function inserts tape data
params:
data, string that represents the command to be inserted

this function parses data and then inserts the pit to the objects array as [xpos,ypos,-2]

"""
def insertTape(data):
    global position
    global objects
    clean = data.replace('%','')
    vector = []
    
    if clean[1] == '1' and clean [2] == '1': #determine which sensor then get vector of itsposition
        vector = vectorize(angle,16)
    elif clean[1] == '1':
        vector = vectorize(angle-10,16)
    elif clean[2] == '1':
        vector = vectorize(angle+10,16)
    elif clean[3] == '1':
        vector = vectorize(angle-60,16)
    elif clean[0] == '1':
        vector = vectorize(angle+60,16)
        
    objects.append([vector[0]+position[0],vector[1]+position[1],-2]) #add tape to objects relative to curr pos
        
        
"""
this function inserts bump data
params:
data, string that represents the command to be inserted

this function parses data and then inserts the pit to the objects array as [xpos,ypos,1]

"""
def insertBump(data):
    global matrix
    global position
    global objects
    clean = data.replace('#','') #clean data string
    vector = []
    if clean[0] == '1' and clean [1] == '1':#determine where bump came from then vectorize it
        #front'''
        vector = vectorize(angle,16)
    elif clean[0] == '1':
        vector = vectorize(angle+45,17)
    elif clean[1] == '1':
        vector = vectorize(angle-45,17)
        
    objects.append([vector[0]+position[0],vector[1]+position[1],1])#add bump to objects relative to curr pos

"""
this function sends strings to the parsechar method
params:
sending, string that represents the raw input from socket

this function simply takes a string and sends every character sequentially to the parse function

"""
def test(sending):
    for char in sending:
        parseChar(char)

	
"""
this function plots the objects array

this functionuses pandas dataframe to draw all the objects onto a scatter plot representing our robot

"""
def plotit():
    global objects
    plt.close()#close open graphs
    df = pandas.DataFrame(objects, columns=['x','y',"type"]) #prepframe
    ax1 = df.plot.scatter(x='x',y='y',c='type',colormap='plasma') #graph function
    plt.show() #show graph
    return
    
	
#open socket socket.AF_INET,
s = socket.socket(socket.AF_INET) #prepare socket connection
ip = socket.gethostbyname('192.168.1.1') #find cybot
print("connecting to socket")
s.connect((ip,288)) #connectto cybot overport 288
print("connected")
lastpressed = '' #used to not send commands extra times
while True:#making a loop
    try: #used try so that if user pressed other than the given key error will not be shown
        if keyboard.is_pressed('w'):#if key 'w' is pressed send w
            if lastpressed is not 'w':
                lastpressed = 'w'
                print('You Pressed W Key!')
                s.send(b'w')
                sleep(2)
                data = s.recv(1024).decode()
                print(data)
                print(len(data))
                test(data)
                #print(int(data[5:])/10.0)
                #print(data[5:])
        elif keyboard.is_pressed('a'):#if key 'a' is pressed send a (turn left)
            if lastpressed is not 'a':
                lastpressed = 'a'
                print('You Pressed A Key!')
                s.send(b'a')
                #turn left
                sleep(2)
                turn(1)
        elif keyboard.is_pressed('s'):#if key 's' is pressed send s (back up)
            if lastpressed is not 's':
                lastpressed = 's'
                print('You Pressed S Key!')
                s.send(b's')
                sleep(2)
                insertMove(-10)
                plotit()
        elif keyboard.is_pressed('d'):#if key 'd' is pressed send d (turn right)
            if lastpressed is not 'd':
                lastpressed = 'd'
                print('You Pressed D Key!')
                s.send(b'd')
                #turn right
                turn(-1)
                sleep(2)
        elif keyboard.is_pressed('q'):#if key 'q' is pressed send q (scan)
            if lastpressed is not 'q':
                lastpressed = 'q'
                print('You Pressed Q Key!')
                s.send(b'q')
                sleep(11)
                data = s.recv(2048).decode()
                test(data)
                print(round(position[0],2),round(position[1],2),angle)
                plotit()
                
            
            
        else:
            lastpressed = ''
            pass
    except:
        break

