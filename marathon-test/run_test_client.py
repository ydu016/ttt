#!/usr/bin/env python

import os
import subprocess
import rpyc

if __name__ == "__main__":
    os.system("rm -rf ./MY_LOGS/*")
    conn = rpyc.connect("192.168.0.120", 30240)       ## 102.168.13.33 = 192.168.0.120
    test_num = 0
    while(True):
        test_count = 0
        while(test_count < 100):
            test_count += 1
            test_num += 1
            conn.root.start_listen()
            os.system("ib_send_lat 192.168.13.33 -a -F -d rxe0 > ./MY_LOGS/" + str(test_num))

        test_count = 0
        while(test_count < 100):
            test_count += 1
            test_num += 1
            subprocess.Popen("ib_send_lat -d rxe0 -a -F > ./MY_LOGS/" + str(test_num), shell = True)
            conn.root.start_send()

    conn.close()
