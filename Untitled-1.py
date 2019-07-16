from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
import requests
import time
import os
import re
import platform
from lxml import etree
from datetime import datetime
driver=None
main_page_url='http://piyao.sina.cn/'
headers = {'User-Agent':'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) '
                        'Chrome/51.0.2704.63 Safari/537.36'}
cookies = None
request_session = None
if platform.system()=='Windows':
    chrome_driver_path = "chromedriver.exe"
elif platform.system()=='Linux' or platform.system()=='Darwin':
    chrome_driver_path = "./chromedriver"
else:
    print('Unknown System Type. quit...')
            

chrome_options=Options()
chrome_options.add_argument('disable-gpu')
driver=webdriver.Chrome(chrome_options=chrome_options,executable_path=chrome_driver_path)

driver.get(main_page_url)
time.sleep(1)

titles=driver.find_elements_by_xpath('//div[@class="left_title"]')
    
for t in titles:
    print(t.text)    
quit()    