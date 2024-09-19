import json
import sys
import os

jfile = sys.argv[1];
jstring = sys.argv[2];

data = None
# Open and read the JSON file
with open(jfile, 'r') as file:
    data = json.load(file)

if ( data == None ):
    os.exit(-1);

jstring = "data" + jstring;
try:
    ret =  eval(jstring)
except:
    os.exit(-1);

print(ret);
