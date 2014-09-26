#!/usr/bin/python

'''

This is simple redif parser module for rrslib. This parser is similiar to
referparser.py in rrslib by S.Heller. Some of classes are empty for documentation.
More info about redif file format:
                http://openlib.org/acmes/root/docu/redif_1.html

'''

import re
from rrslib.db.model import *
from rrslib.extractors.entitywrapper import EntityWrapper
from rrslib.web.httptools import is_url_valid
from rrslib.extractors.rrsregex import ISBNre

__modulename__ = "redifparser"
__author__ = "Michal Lukac"
__email__ = "xlukac05@stud.fit.vutbr.cz"
__date__ = "$10-December-2011 21:29:10$"

# How to map 'TEMPLATE-TYPE' to publication['type'].
REDIF_TEMPLATE_TYPES = {'REDIF-ARCHIVE 1.0':'misc','REDIF-SERIES 1.0':'book',
'REDIF-PAPER 1.0':'article','REDIF-ARTICLE 1.0':'article','REDIF-SOFTWARE 1.0':'misc',
'REDIF-BOOK 1.0':'book','REDIF-CHAPTER 1.0':'article','REDIF-PERSON 1.0':'misc',
'REDIF-INSTITUTION 1.0':'misc',}

class REDIFContentHandler(object):
    """
    Template for all ris-like content-handlers. Semantics is taken from
    http://openlib.org/acmes/root/docu/redif_1.html
    """

    def start_entry(self):
        """ New entry detected. Delimeters are '\n\n' or '\r\n\r\n'. """
        pass

    def end_entry(self):
        """
        End of current entry detected. After this method is called start_entry().
        """
        pass

    def get_data(self):
        """ Return buffer. """
        pass

    def template_type(self, t):
        """ String 'Template-type:' code. """
        pass

    def title(self, title):
        """ String after 'Name:' code. """
        pass

    def author(self, author):
        """ String after 'Author:|Author-Person:|Author-Name' code. """
        pass

    def languague(self,lang):
        """ String after 'Languague:' code. """
        pass

    def handle(self,handle):
        """ String after 'Handle:' code. """
        pass

    def date(self, date):
        """ String after 'Creation-Date:' code. """
        pass

    def abstract(self, abstract):
        """ String after 'Abstract:' code. """
        pass

    def volume(self, vol):
        """ String after 'Volume:' code. """
        pass

    def notes(self, notes):
        """ String after 'Note:'|'Description:' code. """
        pass

    def keywords(self, keys):
        """ String after 'Keywords:' code. """
        pass

    def issn(self, numb):
        """ String after 'ISSN:' code. """
        pass

    def url(self, url):
        """ String after 'URL:' code. """
        pass

    def email(self, email):
        """ String after 'Maintainer-Email:' code. """
        pass

    def isbn(self, isbn):
        """ String after 'ISBN:' code. """
        pass

    def publisher(self, publisher):
        """ String after 'Provider-Name:' code. """
        pass

    def aphone(self, aphone):
        """ String after 'Author-Phone:' code. """
        pass

    def aperson(self, aperson):
        """ String after 'Author-Persone:' code. """
        pass

    def notify(self, notify):
        """ String after 'Notification:' code. """
        pass

    def available(self, available):
        """ String after 'Availability:' code. """
        pass

    def series(self, series):
        """ String after 'Series:' code. """
        pass

    def version(self, version):
        """ String after 'Version:' code. """
        pass

    def isbn(self, isbn):
        """ String after 'ISBN:' code. """
        pass

#-------------------------------------------------------------------------------
# End of class REDIFContentHandler
#-------------------------------------------------------------------------------

class ListedDict(dict):
    def __setitem__(self, key, value):
        if key in self:
            if isinstance(value, (list, tuple)):
                self[key].extend(list(value))
            else:
                self[key].append(value)
        else:
            dict.__setitem__(self, key, [value])

#-------------------------------------------------------------------------------
# End of class ListedDict
#-------------------------------------------------------------------------------

class _REDIFScanner(object):
    def set_source(self, s):
        """ Divide each entry. """
        s = re.sub('[ \t\-]{2,}',' ',s)
        self.source = re.sub("(\r?\n){2,}", "\n\n", s)
        self.entries = self.source.split("\n\n")
        self.entry_counter = 0

    def _parse_entry(self, e):
        attrs = re.findall(".+[^\n]", e.strip(), re.I)
        d = ListedDict()
        key = ""
        temp = ""
        index = 0
        fouded = 0
        for a in attrs:
            a = a.strip()
            reg = re.search('.[^:]+:',a)
            if reg:
                d[reg.group(0)] = a[len(reg.group(0)):]
        return d

    def get_entry(self):
        if len(self.entries) <= self.entry_counter:
            return None
        r = self._parse_entry(self.entries[self.entry_counter])
        self.entry_counter += 1
        return r

#-------------------------------------------------------------------------------
# End of class REDIFScanner
#-------------------------------------------------------------------------------

class REDIFReader(object):
    def __init__(self):
        self.handler = None
        self.scanner = _REDIFScanner()
        self.method_map = {'TEMPLATE-TYPE:':'template_type','HANDLE:':'handle','NAME:':'title',
        'MAINTAINER-EMAIL:':'email','DESCRIPTION:':'notes','URL:':'url','KEYWORDS:':'keywords',
        'ISSN:':'issn', 'AUTHOR-NAME:':'author','LANGUAGUE:':'languague','ABSTRACT:':'abstract',
        'CREATION-DATE:':'date','NOTE:':'notes','FILE-URL:':'url','JOURNAL:':'title',
        'AUTHOR:':'author','AUTHOR-PERSON:':'author','VOLUME:':'volume','BOOK-TITLE:':'title',
        'ISBN:':'isbn','PROVIDER-NAME:':'publisher','TITLE:':'title','AUTHOR-PHONE:':'aphone',
        'AUTHOR-PERSONE:':'aperson','NOTIFICATION:':'notify','AVAILABILITY:':'available',
        'SERIES:':'series','VERSION:':'version'}

    def set_content_handler(self, handler):
        self._handler = handler

    def parse(self, stream):
        self.parse_string(stream.read())

    def parse_string(self, string):
        if self._handler == None:
            raise RuntimeError("No handler found. Set appropriate handler by"\
                               " set_content_handler() method.")
        self.scanner.set_source(string)
        e = self.scanner.get_entry()
        while e is not None:
            if len(e.keys()) != 0:
                self._handler.start_entry()
                for code in e:
                    if code.upper() in self.method_map:
                        method = getattr(self._handler, self.method_map[code.upper()])
                        for val in e[code]:
                            method(val)
                self._handler.end_entry()
            e = self.scanner.get_entry()

#-------------------------------------------------------------------------------
# End of class REDIFReader
#-------------------------------------------------------------------------------

class REDIF2ModelHandler(REDIFContentHandler):
    publ = None
    buffer = []
    entitywrapper = EntityWrapper()
        
    def __init__self(self):
        self.publ = None
        self.buffer = []
        self.entitywrapper = EntityWrapper()

    def start_entry(self):
        """ New entry detected. After this method is called start_entry(). """
        self.publ = RRSPublication()

    def end_entry(self):
        """ End of current entry detected. """
        self.buffer.append(self.publ)

    def get_data(self):
        """ Return buffer. """
        return self.buffer

    def template_type(self, t):
        """ String after 'Name:' code. """
        rediftype = t.upper().strip()
        if rediftype in REDIF_TEMPLATE_TYPES:
            rrstype = REDIF_TEMPLATE_TYPES[rediftype]
            self.publ['type'] = RRSPublication_type(type=rrstype)

    def author(self, author):
        """ String after 'Author:|Author-Person:|Author-Name:' code. """
        pers = self.entitywrapper.wrap(author, RRSPerson)
        rel = RRSRelationshipPersonPublication()
        rel.set_entity(pers)
        rel['editor'] = False
        self.publ['person'] = rel

    def date(self, date):
        """ String after 'Handle:' code. """
        year = re.search("[0-9]+", date)
        if year:
            self.publ['year'] = int(year.group(0))

    def title(self, title):
        """ String after 'Name:' code. """
        self.publ['title'] = title

    def publisher(self, publisher):
        """ String after 'Provider-Name:' code. """
        self.publ['publisher'] = self.entitywrapper.wrap(issuer, RRSOrganization)

    def isbn(self, isbn):
        """ String after 'ISBN:' code. """
        m = re.search(ISBNre + ",? ?", sn)
        if m and m.group(0):
            self.publ['isbn'] = m.group(1)

    def abstract(self, abstract):
        """ String after 'Abstract:' code. """
        self.publ['abstract'] = re.sub("[\n\t]+", " ", abstract)

    def volume(self, vol):
        """ String after 'Volume:' code. """
        try:
            self.publ['volume'] = int(vol)
        except:
            return

    def series(self, series):
        """ String after 'Series:' code. """
        self.publ['series'] = self.entitywrapper.wrap(series, RRSPublication_series)

    def notes(self, notes):
        """ String after 'Note:'|'Description:' code. """
        self.publ['note'] = notes

    def keywords(self, keys):
        """ String after 'Keywords:' code. """
        self.publ['keyword'] = self.entitywrapper.wrap(keys, RRSKeyword)

    def url(self, url):
        """ String after 'URL:' code. """
        rel = RRSRelationshipPublicationUrl()
        rel.set_entity(RRSUrl(link=url))
        self.publ['url'] = rel;

    def languague(self, lang):
        """ String after 'Languague:' code. """
        pass

    def handle(self, handle):
        """ String after 'Handle:' code. """
        pass

    def issn(self, numb):
        """ String after 'ISSN:' code. """
        try:
            self.publ['number'] = int(numb)
        except ValueError:
            pass

    def isbn(self, isbn):
        """ String after 'ISBN:' code. """
        m = re.search(ISBNre + ",? ?", isbn)
        if m and m.group(0):
            self.publ['isbn'] = m.group(1)


    def email(self, email):
        """ String after 'Maintainer-Email:' code. """
        pass

    def aphone(self, aphone):
        """ String after 'Author-Phone:' code. """
        pass

    def aperson(self, aperson):
        """ String after 'Author-Person:' code. """
        pass

    def notify(self, notify):
        """ String after 'Notification:' code. """
        pass

    def available(self, available):
        """ String after 'Availability:' code. """
        pass

    def series(self, series):
        """ String after 'Series:' code. """
        pass

    def version(self, version):
        """ String after 'Version:' code. """
        pass

#-------------------------------------------------------------------------------
# End of class REDIF2ModelHandler
#-------------------------------------------------------------------------------

if __name__ == '__main__':
    f = open('./tests/test3.txt',"r")
    data = f.read().decode('utf-8','ignore')
    rr = REDIFReader()
    redif2mh = REDIF2ModelHandler()
    rr.set_content_handler(redif2mh)
    rr.parse_string(data)
    from rrslib.xml.xmlconverter import Model2XMLConverter
    cc = Model2XMLConverter()
    cc.convert(redif2mh.get_data())
