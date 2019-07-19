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
list = []
listt = []
res={}
driver.get(main_page_url)
time.sleep(1)

for i in range(0,20):

    js="window.scrollTo(0,document.body.scrollHeight)"
    driver.execute_script(js)
    time.sleep(1)
#多翻几页网页增加内容

like = driver.find_elements_by_xpath('//div[@class="like_text"]')
    
for i in like:
   
    listt.append(int(i.text))
    #点赞数为字符串转为int型存入列表
    
 
titles=driver.find_elements_by_xpath('//div[@class="left_title"]')

for t in titles:
    
    list.append(t.text)
#分别爬取内容，点赞数     
res =dict(zip(listt,list))
#合并为字典
s=sorted(res.items(),key=lambda x:x[0])    
e=len(s)
#字典为正序排序，倒序输出十个即为点赞前10
for i in range(e-1,e-11,-1):

    print(s[i])
    print("\n")
quit()   