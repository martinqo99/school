'''

  Project : ccsb_down.py, Downloading data from CCSB
  Author  : Michal Lukac, xlukac05@stud.fit.vutbr.cz, BUT FIT

  This script download all available collections from CCSB and convert
  them to rrs-xml. CCSB (http://liinwww.ira.uka.de/bibliography/) is a webpage
  with collection of computer science bibliographies. Script is working
  with Python2.7 version.

'''

import sys, os, re, urllib2, StringIO
from BeautifulSoup import BeautifulSoup
from rrslib.extractors.bibtex import BibTeXReader, BibTeX2ModelContentHandler
from rrslib.extractors.referparser import ReferReader, Refer2ModelHandler
from rrslib.xml.xmlconverter import Model2XMLConverter

# UrlAdress of collections depends on category.
g_url = 'http://liinwww.ira.uka.de/bibliography/'

# List of categories used in url adress.
g_categories = ['Ai', 'Compiler','Database','Distributed',
                'Graphics','LogicProgramming','Math','Neural',
                'Object','Os','Parallel','SE','Theory',
                'Typesetting','Misc','Techreports']

# List of bibtex key-names for bibliography recognition
g_bibkeys = ['@article','@book','@booklet','@conference','@inbook','@incollection',
             '@inproceedings','@manual','@mastersthesis','@misc','@phdthesis',
             '@proceedings','@techreport','@unpublished']

# Count of downloaded collections.
g_numb = 0
g_total = 0

# Path where you want to download files.
g_path = './Downloaded/'

# How many files is max for bibtex divide.
g_bibfiles = 10

# How many files is max for refer divide.
g_referfiles = 20

# Param for converting, downloading, creating log.
g_convert = False
g_download = True
g_log = False
g_divide = True

def convertFromRefer(file_c):
  ''' Convert files from refer format to rrs-xml. '''
  global g_path
  print 'Converting from refer file:' + g_path + file_c
  try:
    # Open refer file.
    f = open(g_path+file_c, "r")
    rr = ReferReader()
    r2mh = Refer2ModelHandler()
    rr.set_content_handler(r2mh)
    # Set parse method.
    rr.parse(f)
    output = StringIO.StringIO()
    cc = Model2XMLConverter(stream=output)
    filexml = open(g_path+re.sub('\.ref.*',r'.xml',file_c),'w')
    cc.convert(r2mh.get_data())
    output.seek(0)
    # Writing in xml.
    filexml.write(output.getvalue().encode('utf-8'))
    filexml.close()
    output.close()
    os.system("rm "+g_path+file_c+" 2>/dev/null")
    f.close()
  except:
    print 'Error converting refer file: ' + g_path + file_c
    os.system("rm "+g_path+re.sub('\.ref.*',r'.xml',file_c)+" 2>/dev/null")
    os.system("rm "+g_path+file_c+" 2>/dev/null")

def convertFromBibtex(file_c):
  ''' Convert files from bibtex format to rrs-xml. '''
  global g_path
  print 'Converting from bibtex file: ' +g_path + file_c
  try:
    # Open bibtex file.
    f = open(g_path+file_c, mode='r')
    br = BibTeXReader()
    h = BibTeX2ModelContentHandler()
    br.set_content_handler(h)
    # Set parse method.
    br.parse(f)
    output = StringIO.StringIO()
    cc = Model2XMLConverter(stream=output)
    filexml = open(g_path+re.sub('\.bib.*',r'.xml',file_c),'w')
    cc.convert(h.data())
    output.seek(0)
    # Writing in xml.
    filexml.write(output.getvalue().encode('utf-8'))
    filexml.close()
    output.close()
    os.system("rm "+g_path+file_c+" 2>/dev/null")
    f.close()
  except:
    print 'Error converting bibtex file: ' +g_path + file_c
    os.system("rm "+g_path+re.sub('\.bib.*',r'.xml',file_c)+" 2>/dev/null")
    os.system("rm "+g_path+file_c+" 2>/dev/null")

def convertFiles():
  ''' Get files from directory and call converting functions. '''
  global g_path
  for file_c in os.listdir(g_path):
    # if file_i is refer document.
    if re.search('\.ref.*',file_c):
      convertFromRefer(file_c)
    # if file_i is bibtex document.
    else:
      convertFromBibtex(file_c)

def divideReferFile(url):
  ''' Divide file defined by url to smaller files. '''
  global g_referfiles, g_total
  count = 0
  numb_file = 0
  # Construct filename.
  filename = url[url.rindex('/'):]
  filename1 = url[:url.rindex('/')+1] + filename[:filename.index('_')+1]
  file1 = open(filename1+str(numb_file)+filename[filename.index('_'):],'w')
  # Go through all lines in file.
  for line in open(url):
    # If there is empty line citation count++.
    if line.strip() == '':
      count = count + 1
    # If is citation count in file maximum -> create next file.
    if count == g_referfiles:
      count = 0
      file1.close()
      g_total = g_total + 1
      numb_file = numb_file + 1
      file1 = open(filename1+str(numb_file)+filename[filename.index('_'):],'w')
    file1.write(line)
  file1.close()
  os.remove(url)

def divideBibtexFile(url):
  ''' Divide file defined by url to smaller files. '''
  global g_bibkeys, g_bibfiles
  count = 0
  numb_file = 0
  # Construct filename
  filename = url[url.rindex('/'):]
  filename1 = url[:url.rindex('/')+1] + filename[:filename.index('_')+1]
  file1 = open(filename1+str(numb_file)+filename[filename.index('_'):],'w')
  # Go through all lines in file.
  for line in open(url):
    for i in range(len(g_bibkeys)):
      # If line has specified key-word.
      if g_bibkeys[i] in line.lower():
        count = count + 1
        i = len(g_bibkeys[i])
      # If is citation count in file maximum -> create next file.
      if count == g_bibfiles:
        count = 0
        file1.close()
        numb_file = numb_file + 1
        file1 = open(filename1+str(numb_file)+filename[filename.index('_'):],'w')
    file1.write(line)
  file1.close()
  os.remove(url)

def divideFile(url_path):
  ''' Call divide file function depends on refer/bibtex file. '''
  if re.search('\.ref.*',url_path):
    divideReferFile(url_path)
  else:
    divideBibtexFile(url_path)

def extractFile(file_path):
  ''' Extract file (file_path). '''
  print 'Extracting ' + file_path
  os.system("gunzip " + file_path)
  if os.path.exists(file_path):
    os.remove(file_path)

def downloadFile2(url_path,filename):
  global g_numb, g_path, g_total, g_divide
  ''' Download file from url_path. '''
  # Open and read file from web.
  file = urllib2.urlopen(url_path+filename)
  g_numb = g_numb + 1
  g_total = g_total + 1
  # Write file on disk.
  output = open(g_path+str(g_numb)+'_'+filename,'w')
  output.write(file.read())
  output.close()
  if g_divide == True:
    divideFile(g_path+str(g_numb)+'_'+filename)
  print 'Downloading from ' + url_path + filename

def downloadFile(url_path,filename):
  ''' Download file from url_path. '''
  global g_numb, g_path, g_total, g_divide
  try:
    # If url contains file/
    if url_path.split('/')[-1]:
      # If is html.
      if re.search(r'.htm',url_path):
        return True
      file = urllib2.urlopen(url_path)
      g_numb = g_numb + 1
      g_total = g_total + 1
      url = g_path+str(g_numb)+'_'+url_path.split('/')[-1]
      output = open(url,'w')
      output.write(file.read())
      output.close()
      # If is compressed gz file.
      if re.search(r'.gz$',url):
        extractFile(url)
      else:
        if g_divide == True:
          divideFile(url)
      return False
    else:
      # If is ftp link.
      if re.search(r'ftp',url_path):
        findLinkInFtp(url_path,filename)
      return False
  except:
    return True

def findLinkInFtp(link, filename):
  ''' This function try to find link to file in ftp. '''
  try:
    # Open ftp.
    file2 = urllib2.urlopen(link)
    output = open("test.txt",'w')
    output.write(file2.read())
    output.close()
    file1 = open("test.txt")
    # Gou through all ftp files and search for bibtex/refer.
    for line in file1:
      if filename in line:
        names = line.split(' ')
        names = names[len(names) -1 ].split('\n')
        if '.bib' in names[0] or '.bibtex' in names[0] or '.txt' in names[0] or '.refer' in names[0]:
          downloadFile2(link, names[0])
    file1.close()
    os.system("rm test.txt")
  except:
    print 'Error with ftp.'

def findLinkInPage(biblio_page,link):
  ''' Find link of bibtex file or gzip in page. '''
  files = link.split('/')
  files = files[len(files)-1]
  filename = files.split('.')
  if 'Local copy' in biblio_page:
    downloadFile(re.sub('.html$','.bib.gz',link),filename[0])
  elif 'Original source:' in biblio_page:
    soup = BeautifulSoup(biblio_page)
    link = soup.find(text='Original source:')
    if downloadFile(link.findNext('a')['href'],filename[0]):
      if 'Other sites:' in biblio_page:
        link = soup.find(text='Other sites:')
        if downloadFile(link.findNext('a')['href'],filename[0]):
          print 'Error downloading ' + link.findNext('a')['href']

def downloadBiblioPage(biblio_link):
  ''' Get bibliography link. '''
  try:
    response = urllib2.urlopen(biblio_link)
    return response.read()
  except:
    print 'Error downloading collection page.'

def getBeautifulLinks(html):
  ''' Get links of bibliography webpage from one category. '''
  soup = BeautifulSoup(html)
  # Get better looking html.
  links = '<html><head></head><body>' + str(soup.findAll('table')) + '</body></html>'
  soup = BeautifulSoup(links)
  return soup

def getCollections(html,category):
  ''' Function getCollections process category page. '''
  try:
    soup = getBeautifulLinks(html)
    # Go through all links in category page.
    for i in range(len(soup('a'))):
      biblio_link = g_url + category + '/' + str(soup('a')[i]['href'])
      print('Downloading ' + g_url + category + '/' + str(soup('a')[i]['href']))
      biblio_page = downloadBiblioPage(biblio_link)
      findLinkInPage(biblio_page,biblio_link)
  except:
    print 'Error downloading collection page!'

def downCategoryPage(category):
  ''' Download html category page. '''
  response = urllib2.urlopen(g_url+category+'/index.html')
  html = response.read()
  print('Searching in category page: '+g_url+category+'/index.html')
  getCollections(html,category)

def startDownload():
  ''' Iterating categories and download html. '''
  global g_categories
  try:
    # Go through all categories.
    for category in g_categories:
      downCategoryPage(category)
  except:
    print 'Error downloading category page.'

def createDir():
  ''' Create directory for downloaded files. '''
  global g_path
  cwd = os.getcwd()
  if g_path == './Downloaded/':
    if not os.path.exists(cwd+'/Downloaded/'):
      os.mkdir(cwd+'/Downloaded/')
  else:
    if not os.path.exists(g_path):
      os.mkdir(g_path)

def printHelp():
  ''' Just help function is here. '''
  print('''This script download all available collections from CCSB.
           -h : Show help.
           -p "/path/where/you/want/download/files"
           -q : Suppress output information of script but create log file.
           -c : Convert downloaded collections from bibtex/refer to rrs-xml.
           -d : Do not download files. Useful with -c.
           -i : Supress dividing file in smaller pieces when is download performing.''')

def prepareForConvert():
  ''' Get files from directory and call converting functions. '''
  global g_path
  for file_c in os.listdir(g_path):
    if os.path.getsize(g_path+file_c) > 100000L:
      divideFile(g_path + file_c)

def processArguments():
  ''' Process arguments of program. '''
  global g_path, g_convert, g_download, g_log, g_divide
  for i in range(len(sys.argv)):
    if sys.argv[i] == '-p':
      if sys.argv[i+1]:
        g_path = sys.argv[i+1]
      else:
        print "Error with argument path!"
        sys.exit(1)
    if sys.argv[i] == '-q':
      g_log = True
    if sys.argv[i] == '-h':
      printHelp()
      sys.exit(0)
    if sys.argv[i] == '-c':
      g_convert = True
    if sys.argv[i] == '-d':
      g_download = False
    if sys.argv[i] == '-i':
      g_divide = False

def main():
  ''' First function of script. Call other functions... '''
  global g_convert, g_download, g_numb, g_total, g_divide
  processArguments() 
  createDir()
  # if is param for create log file
  if g_log == True:
    lstdout = sys.stdout
    flog = open('log.txt','w')
    sys.stdout = flog
  # if is downloading(default)
  if g_download == True:
    startDownload()
    print 'Total files: ' + str(g_total-g_numb)
  # if is param for converting to rrs-xml
  if g_convert == True:
    if g_divide == True:
      print 'Preparing for converting. It could take few minutes.'
      prepareForConvert()
    convertFiles()
  if g_log == True:
    flog.close()
    sys.stdout = lstdout

if __name__ == "__main__":
  main()
