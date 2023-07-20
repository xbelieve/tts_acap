#!/usr/bin/python
# coding=UTF-8

import glob
import time
import sys
import os
import logging
from logging.handlers import TimedRotatingFileHandler

import argparse
import requests
from requests.auth import HTTPDigestAuth

def main():
    name   = "speaker_trigger_log"
    #logger = openlog(name)

    parser = argparse.ArgumentParser(description="Send a trans with a given"
        "name to a given ACS instance")

    parser.add_argument("-t", "--trans", dest="trans", action="store",
        help="trans", required=True)
    parser.add_argument("-i", "--ipname", dest="ipname", action="store",
        help="IP address / hostname and port for ACS server", required=True)
    parser.add_argument("-u", "--user", dest="user", action="store",
        help="username", required=True)
    parser.add_argument("-p", "--password", dest="password", action="store",
        help="password", required=True)
    
    args = parser.parse_args()

    #logger.info(f"Called with arguments: "
     #   f"trans= {args.trans} ipname= {args.ipname}")

    # Build cURL command from arguments
    #cmd = f"curl --digest --user {args.user} --request GET \"http://{args.ipname}/local/tts_go/events?trans=歡迎{args.trans}參觀展示中心&lan=zh-TW\""

    #url = f"http://{args.ipname}/local/tts_go/events?trans=歡迎{args.trans}參觀展示中心&lan=zh-TW"
    #url = 'https://httpbin.org/digest-auth/auth/user/pass'
    url = 'http://%s/local/tts_go/events?trans=歡迎%s參觀展示中心&lan=zh-TW' % (args.ipname, args.trans)
    print (url)
    requests.get(url, auth=HTTPDigestAuth(args.user, args.password ))


    #logger.info(cmd)
    print (url)
    # Run system command
    #os.system(cmd)

if __name__ == '__main__':
    main()


