#!/usr/bin/env python

import os
import subprocess

from rpyc import Service
from rpyc.utils.server import ThreadedServer

class TestService(Service):
    """ Test Service """

    def exposed_start_listen(self):
        subprocess.Popen("ib_send_lat -d rxe0 -a -F", shell = True)

    def exposed_start_send(self):
        os.system("ib_send_lat 192.168.13.22 -a -F -d rxe0")

if __name__ == "__main__":
    srv = ThreadedServer(TestService, port = 30240, auto_register = False)
    srv.start()
    print ">>> Server Started!"
