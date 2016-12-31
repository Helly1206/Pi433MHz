#!/usr/bin/python
# -*- coding: utf-8 -*-
#########################################################
# SERVICE : client.py                                   #
#           Python client test progr for 433 MHz daemon #
#           I. Helwegen 2016                            #
#########################################################

####################### IMPORTS #########################
import thread
import Pi433MHzif

#########################################################

####################### GLOBALS #########################
MAXSIZE=3
inputstring = ""

#########################################################

#########################################################
# Class : Format                                        #
#########################################################
class Format(object):
    def decode(self, data, maxsize):
        array= []
        res= -1

        if data:
            i=0
            d=data
            while d and (i<maxsize):
                pos = d.find("!")
                try:
                    v = int(d[0:pos])
                except:
                    v = 0
                array.append(v)
                d = d[pos+1:]
                i += 1
            res = i

        return res, array

    def encode(self, array):
        data = ""
        res = -1

        if array:
            res += 1
            for i in range (0,len(array)):
                try:
                    v = str(array[i])
                except:
                    v = str(int(0))
                data += v
                data += "!"
                res += 1
        return res, data

    def testarray(self, array):
        bres = False

        res, enc = self.encode(array)

        if (res > 0):
            res2, narr = self.decode(enc, res+2)
            if (res == res2):
                res3 = 0
                for i in range(0, res):
                    if (array[i] == narr[i]):
                        res3 += 1
                if (res == res3):
                    bres = True
        return bres

    def teststring(self, data, maxsize):
        bres = False

        res, narr = self.decode(data, maxsize)

        if (res > 0):
            res2, enc = self.encode(narr)
            if (res == res2):
                if (enc == data):
                    bres = True
        return bres


####################### THREAD ##########################
def MyThread():
    global inputstring
    while (1):
        inputstring = raw_input("-->")


######################## MAIN ###########################
format = Format()
Pi433MHzclient = Pi433MHzif.Pi433MHzif()
cont = True

reply = ""
inputstringcpy = ""

print "Python TestClient for Pi433MHzd"
print "Enter your input depending on switch type"
print "raw: 'xxxxxx' (= integer number)"
print "action: 'syscode!devcode!on!' (on= 0 or 1)"
print "blokker: 'device!on!' (on= 0 or 1)"
print "kaku: 'address!device!on!' (on= 0 or 1)"
print "elro: 'syscode!devcode!on!' (on= 0 or 1)"

try:
   thread.start_new_thread(MyThread, ( ))
except:
   print "ERROR Creating thread"

while (cont):
    if inputstring and (inputstring != inputstringcpy):
        inputstringcpy = inputstring
        if inputstring.endswith("!"):
            data = inputstring
        else:
            data = inputstring + "!"
        if format.teststring(data, MAXSIZE):
            size, array = format.decode(data, MAXSIZE)
            if (size > 0):
                res = Pi433MHzclient.WriteMessage(array, size)
                if (res < 0):
                    cont = False
        else:
            print "Invalid input string"

    if (cont):
        res, array = Pi433MHzclient.ReadMessage(MAXSIZE)
        if (res < 0):
            cont = False
        if (res > 0):
            res, data = format.encode(array)
            print "<--" + data

del Pi433MHzclient
del format