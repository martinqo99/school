#!/usr/bin/python

'''

This is simple ris parser module for rrslib. This parser is similiar to
referparser.py in rrslib by S.Heller. Some of classes are empty for documentation.
More info about ris file format:
        format http://www.researcherid.com/resources/html/help_upload.htm

'''

import re
from rrslib.db.model import *
from rrslib.extractors.entitywrapper import EntityWrapper
from rrslib.web.httptools import is_url_valid
from rrslib.extractors.rrsregex import ISBNre

__modulename__ = "risparser"
__author__ = "Michal Lukac"
__email__ = "xlukac05@stud.fit.vutbr.cz"
__date__ = "$21-November-2011 21:29:10$"

#RIS_REFERENCE_TYPES = {'ABST':'Abstract','ADVS':'Audiovisual material',
#'ART':'Art Work','BOOK':'book','CASE':'Case','CHAP':'Book chapter',
#'COMP':'Computer program','CONF':'Conference proceeding','CTLG':'Catalog',
#'DATA':'Data file','ELEC':'Electronic Citation','GEN':'Generic',
#'HEAR':'Hearing','ICOMM':'Inernet Communication','INPR':'In Press',
#'JFULL':'Journal (full)','JOUR':'Journal','MAP':'Map','MGZN':'article',
#'MPCT':'Motion picture','MUSIC':'Music score','NEWS':'Newspaper','PAMP':'Pamphlet',
#'PAT':'Patent','PCOMM':'Personal communication','RPRT':'Report',
#'SER':'Serial publication','SLIDE':'Slide','SOUND':'Sound recording',
#'STAT':'Statute','THES':'Thesis/Dissertation','UNPB':'Unpublished work',
#'VIDEO':'Video recording',}

RIS_TO_RRS_REFERENCE_TYPES = {'ABST':'article','ADVS':'misc',
'BOOK':'book','CASE':'misc','CHAP':'inbook',
'COMP':'misc','CONF':'conference','CTLG':'book',
'Data':'misc','ELEC':'misc','GEN':'misc','INPR':'article',
'JFULL':'book','JOUR':'article','MGZN':'article',
'NEWS':'book','PAMP':'booklet','PAT':'misc',
'PCOMM':'misc','RPRT':'article','SER':'inproceedings',
'SLIDE':'booklet','SOUND':'misc','STAT':'misc',
'THES':'phdthesis','UNPB':'unpublished',}

class RISContentHandler(object):
    """
    Template for all ris-like content-handlers. Semantics is taken from
    http://en.wikipedia.org/wiki/RIS_%28file_format%29
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

    def reference_type(self, t):
        """
        String after TY code: Reference Type.
        One of:Abstract, Audiovisual material, Art Work, Whole book, Case,
        Book chapter, Computer program, Conference proceeding, Catalog, Data file,
        Electronic Citation, Generic, Hearing, Internet Communication, In Press,
        Journal (full), Journal, Map, Magazine article, Motion picture, Music score,
        Newspaper, Pamphlet, Patent, Personal communication, Report, Serial publication
        Slide, Sound recording, Statute, Thesis/Dissertation, Unpublished work,
        Video recording
        """
        pass

    def author(self, author):
        """ String after AU code: Author's name. """
        pass

    def pub_year(self, date):
        """ String after PY code: Publication Year. """
        pass

    def title(self, title):
        """String after TI code : Book title. """
        pass

    def publisher(self, publisher):
        """ String after PB code : publisher. """
        pass

    def sn(self, sn):
        """ String after SN code: ISBN/ISSN. """
        pass

    def abstract(self, abstract):
        """ String after N2 code: Abstract. """
        pass

    def volume(self, vol):
        """ String after VL code: Volume number. """
        pass

    def series(self, series):
        """ String after T3 code: Series title. """
        pass

    def ref_id(self, id):
        """ String after ID code: Reference ID. """
        pass

    def un_ref(self, ref):
        """ String after CT code: Title of unpublished reference. """
        pass

    def prim_author(self, prim):
        """ String after A1 code: Primary author. """
        pass

    def sec_author(self, sec):
        """ String after A2 code: Secondary author. """
        pass

    def prim_date(self, prim):
        """ String after Y1 code: Primary date. """
        pass

    def notes(self, notes):
        """ String after N1 code: Notes. """
        pass

    def keywords(self, keys):
        """ String after KW code: Keywords. """
        pass

    def reprint_stat(self, stat):
        """ String after RP code: Reprint status. """
        pass

    def sp_numb(self, numb):
        """ String after SP code: Start page number. """
        pass

    def endp_numb(self, numb):
        """ String after EP code: Periodical name. """
        pass

    def period_name(self, name):
        """ String after JF JO JA J1 J2 code: """
        pass

    def issue_numb(self, numb):
        """ String after IS code: Issue number. """
        pass

    def sec_title(self, stitle):
        """ String after T2 code: Secondary title. """
        pass

    def city_pub(self, city):
        """ String after CY code: City of publication. """
        pass

    def usr_def(self, usr):
        """ String after U1 U5 code: User defined. """
        pass

    def available(self, avb):
        """ String after AV code: Availability. """
        pass

    def misc(self, misc):
        """ String after M1 M3 code: Misc. """
        pass

    def adress(self, adress):
        """ String after AD code: Adress. """
        pass

    def url(self, url):
        """ String after UR code: Web, url. """
        pass

    def link(self, link):
        """ String after L1 L2 L3 L4 code: Link to pdf, fulltext ... """
        pass

#-------------------------------------------------------------------------------
# End of class RISContentHandler
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

class _RISScanner(object):
    def set_source(self, s):
        s = re.sub('[ \t\-]{2,}',' ',s)
        self.source = re.sub("(\r?\n){2,}", "\n\n", s)
        self.entries = self.source.split("ER")
        self.entry_counter = 0

    def _parse_entry(self, e):
        attrs = re.findall(".+[^\n]", e.strip(), re.I)
        d = ListedDict()
        key = ""
        for a in attrs:
            a = a.strip()
            if re.search("[A-Z0-9]{2}.+",a):
                key = a[0:2].upper()
                d[key] = a[3:]
            else:
                d[key] = ' '.join(d[key]) + ' ' + a[:]
        return d

    def get_entry(self):
        if len(self.entries) <= self.entry_counter:
            return None
        r = self._parse_entry(self.entries[self.entry_counter])
        self.entry_counter += 1
        return r

#-------------------------------------------------------------------------------
# End of class _RISScanner
#-------------------------------------------------------------------------------

class RISReader(object):
    def __init__(self):
        self.handler = None
        self.scanner = _RISScanner()
        self.method_map = {'TY':'reference_type','ID':'ref_id','T1':'title',
        'TI':'title','CT':'un_ref','A1':'author','A2':'author','A3':'author',
        'A4':'author','LB':'label','DA':'pub_year','AU':'author', 'Y1':'prim_date',
        'PY':'pub_year','N1':'notes','KW':'keywords','RP':'reprint_stat',
        'SP':'sp_numb','EP':'endp_numb','JF':'period_name','JO':'period_name',
        'JA':'period_name','J1':'period_name','J2':'period_name','VL':'volume','NV':'volume',
        'C1':'custom','C2':'custom', 'C3':'custom','C4':'custom','C5':'custom',
        'LA':'languague','IS':'issue_numb','T2':'sec_title','CY':'city_pub','PB':'publisher',
        'U1':'usr_def','U5':'usr_def','T3':'series','N2':'abstract','AB':'abstract',
        'SN':'sn','AV':'available','M1':'misc','M3':'misc',
        'AD':'adress','UR':'url','L1':'link','L2':'link',
        'L3':'link','L4':'link','ER':'end_entry'}

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
                    if code in self.method_map:
                        method = getattr(self._handler, self.method_map[code.upper()])
                        for val in e[code]:
                            if re.search("[a-z0-9]+", val, re.I):
                                method(val)
                self._handler.end_entry()
            e = self.scanner.get_entry()

#-------------------------------------------------------------------------------
# End of class RISReader
#-------------------------------------------------------------------------------

class RIS2ModelHandler(RISContentHandler):
    def __init__(self):
        self.publ = None
        self.buffer = []
        self.entitywrapper = EntityWrapper()

    def start_entry(self):
        """ New entry detected. Delimeters are '\n\n' or '\r\n\r\n'. """
        self.publ = RRSPublication()

    def end_entry(self):
        """ End of current entry. """
        self.buffer.append(self.publ)
        self.publ = None

    def get_data(self):
        """ Return buffer. """
        return self.buffer

    def reference_type(self, t):
        """ String after TY code. """
        global RIS_TO_RRS_REFERENCE_TYPES
        ristype = t.upper()
        if ristype in RIS_TO_RRS_REFERENCE_TYPES:
            rrstype = RIS_TO_RRS_REFERENCE_TYPES[ristype]
            self.publ['type'] = RRSPublication_type(type=rrstype)

    def author(self, author):
        """ String after AU code. Author's name. """
        pers = self.entitywrapper.wrap(author, RRSPerson)
        rel = RRSRelationshipPersonPublication()
        rel.set_entity(pers)
        rel['editor'] = False
        self.publ['person'] = rel

    def pub_year(self, date):
        """ String after PY code: Publication Year. """
        year = re.match("\A[0-9]+$", date)
        if year:
            self.publ['year'] = int(year.group(0))

    def title(self, title):
        """ String after TI code: Book title. """
        self.publ['title'] = title

    def publisher(self, publisher):
        """ String after PB code: Publisher. """
        self.publ['publisher'] = self.entitywrapper.wrap(publisher, RRSOrganization)

    def sn(self, sn):
        """ String after SN code: ISBN/ISSN. """
        m = re.search(ISBNre + ",? ?", sn)
        if m and m.group(0):
            self.publ['isbn'] = m.group(1)

    def abstract(self, abstract):
        """ String after N2 code: Abstract. """
        self.publ['abstract'] = re.sub("[\n\t]+", " ", abstract)

    def volume(self, vol):
        """ String afte V2 code : Volume number. """
        try:
            self.publ['volume'] = int(vol)
        except:
            return

    def series(self, series):
        """ String after T3 code: Series title. """
        self.publ['series'] = self.entitywrapper.wrap(series, RRSPublication_series)

    def ref_id(self, id):
        """ String after ID code: Reference ID. """
        pass

    def un_ref(self, ref):
        """ String after CT code: Title of unpublished reference. """
        pass

    def prim_author(self, prim):
        """ String after A1 code: Primary author. """
        pers = self.entitywrapper.wrap(prim, RRSPerson)
        rel = RRSRelationshipPersonPublication()
        rel.set_entity(pers)
        rel['editor'] = False
        self.publ['person'] = rel

    def sec_author(self, sec):
        """ String after A2 code: Secondary author. """
        pers = self.entitywrapper.wrap(sec, RRSPerson)
        rel = RRSRelationshipPersonPublication()
        rel.set_entity(pers)
        rel['editor'] = False
        self.publ['person'] = rel

    def prim_date(self, prim):
        """ String after Y1 code: Primary date. """
        year = re.match("\A[0-9]+$", prim)
        if year:
            self.publ['year'] = int(year.group(0))

    def notes(self, notes):
        """ String after N1 code: Notes. """
        self.publ['note'] = notes

    def languague(self, lang):
        """ String after LA code. """
        pass

    def keywords(self, keys):
        """ String after KW code. Keywords. """
        self.publ['keyword'] = self.entitywrapper.wrap(keys, RRSKeyword)

    def reprint_stat(self, stat):
        """ String after RP code: Reprint status. """
        pass

    def sp_numb(self, numb):
        """ String after SP code: Start page number. """
        self.publ['pages'] = numb

    def endp_numb(self, numb):
        """ String after EP code: Periodical name. """
        pass

    def period_name(self, name):
        """ String after JF code. """
        self.publ['event'] = self.entitywrapper.wrap(name, RRSEvent)
        pass

    def issue_numb(self, numb):
        """ String after IS code: Issue number. """
        try:
            self.publ['number'] = int(numb)
        except ValueError:
            pass
	      
    def sec_title(self, stitle):
        """ String after T2 code: Secondary title. """
        pass

    def custom(self, cust):
        """ String afte C1 code: Custom. """
        pass

    def label(self, lab):
        """ String afte LB code: Label. """
        pass

    def city_pub(self, city):
        """ String after CY code: City of publication. """
        pass

    def usr_def(self, usr):
        """ String after U1 U5 code: User defined. """
        pass

    def available(self, avb):
        """ String after AV code: Availability. """
        pass

    def misc(self, misc):
        """ String after M1 M3 code: Misc. """
        pass

    def adress(self, adress):
        """ String after AD code: Adress. """
        pass

    def url(self, url):
        """ String after UR code: Web, url. """
        rel = RRSRelationshipPublicationUrl()
        rel.set_entity(RRSUrl(link=url))
        self.publ['url'] = rel;

    def link(self, link):
        """ String after L1 L2 L3 L4 code: Link to pdf, fulltext ... """
        pass

#-------------------------------------------------------------------------------
# End of class RIS2ModelHandler
#-------------------------------------------------------------------------------

if __name__ == '__main__':
    f = open('./tests/test10.ris',"r")
    data = f.read()
    rr = RISReader()
    ris2mh = RIS2ModelHandler()
    rr.set_content_handler(ris2mh)
    rr.parse_string(data)
    from rrslib.xml.xmlconverter import Model2XMLConverter
    cc = Model2XMLConverter()
    cc.convert(ris2mh.get_data())
