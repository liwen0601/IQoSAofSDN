import os
import time
lastModify = os.path.getmtime("config.ini")
while True:
    time.sleep(1)
    try:
        lm = os.path.getmtime("config.ini")
        if lm > lastModify:
            lastModify = lm
            os.system("ovs-ofctl del-flows s1")
            os.system("ovs-ofctl add-flow s1 priority=0,actions=CONTROLLER:65535")
    except:
        a=0

