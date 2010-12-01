
def rdf (s):
        return "rdf:" + s

def rdfs (s):
        return "rdfs:" + s

def xsd (s):
        return "xsd:" + s

def dc (s):
        return "dc:" + s

def nie (s):
        return "nie:" + s

def nco (s):
        return "nco:" + s

def nfo (s):
        return "nfo:" + s

def nmm (s):
        return "nmm:" + s

def nao (s):
        return "nao:" + s

def literal (s):
        return "\"%s\"" % (s)

def duration (m, s):
        return literal ("PT%dM%dS" % (m, s))


URI_TEMPLATE = "codethink:%s"

import uuid
import string

def resourceURI ():
        return URI_TEMPLATE % (string.replace(str(uuid.uuid4()), '-', ''))

class Resource:
        def __init__ (self, uri=None):
                self._tups = []
                if uri:
                        self._uri = uri
                else:
                        self._uri = resourceURI ()

                self.atup ('a', rdfs('Resource'))

        def atup (self, predicate, object):
                self._tups.append((self._uri, predicate, object))

        @property
        def uri (self):
                return self._uri

        @property
        def tups (self):
                return self._tups


import random

class NIE (Resource):
        def __init__ (self, of=None, uri=None):
                Resource.__init__ (self, uri)

                self.atup ('a', nie ('InformationElement'))
                self.atup ('a', nie ('DataObject'))

                if (of):
                        self.atup (nie('isLogicalPartOf'), of.uri)

class NAOTag (NIE):
        def __init__ (self, label, of=None):
                NIE.__init__(self, of)

                #self.atup ('a', nao ('Symbol'))
                self.atup ('a', nao ('Tag'))

                self.atup (nao ('prefLabel'), literal (label))

        def isTagFor (self, resource):
                #self.atup (nao ('isTagFor'), resource.uri)
                pass

import datetime

class NFOFileDataObject (NIE):
        def __init__ (self, of=None):
                NIE.__init__ (self, of)

                self.atup ('a', nfo ('FileDataObject'))

                fname = "/com/codethink/test/directory/%s"
                anurl = "file:///com/codethink/test/directory/%s"

                self.atup (nie ('url'), literal(anurl % self.uri))
                self.atup (nfo ('fileName'), literal(fname % self.uri))

                date = datetime.datetime (random.randint (2007, 2009), #Year
                                          random.randint (1, 12), #Month
                                          random.randint (1, 28), #Day
                                          random.randint (0, 23), #Hour
                                          random.randint (0, 59), #Minutes
                                          random.randint (0, 59), #Seconds
                                         )

                self.atup (nfo('fileLastModified'), literal(date.isoformat()))

class NFOVisual (NFOFileDataObject):
        def __init__ (self, of=None):
                NFOFileDataObject.__init__ (self, of)

                self.atup ('a', nfo ('Media'))
                self.atup ('a', nfo ('Visual'))

                self.atup (nfo('width'), random.randint (300, 2000))
                self.atup (nfo('height'), random.randint (300, 2000))
                if (random.randint (0, 2) == 0):
                        self.atup (nfo('device'), literal('NOKIA'))

class NMMPhoto (NFOVisual):
        def __init__ (self, tag=None, of=None):
                NFOVisual.__init__ (self, of)

                self.atup ('a', nfo ('Image'))
                self.atup ('a', nmm ('Photo'))

                self.atup (nie ('mimeType'), literal('image/jpeg'))
                self.atup (nmm ('exposureTime'), random.random())
                self.atup (nmm ('fnumber'), random.uniform(1, 8))
                self.atup (nmm ('focalLength'), random.uniform(5, 50))

                if (tag):
                        tag.isTagFor (self)
                        self.atup (nao ('hasTag'), tag.uri)

class NMMVideo (NFOVisual):
        def __init__ (self, tag=None, of=None):
                NFOVisual.__init__ (self, of)

                self.atup ('a', nmm ('Video'))

                self.atup (nie ('mimeType'), literal('video/x-ms-wmv'))
                self.atup (nfo ('duration'), duration (random.randint (0, 59), random.randint (0, 59)))

                if (tag):
                        tag.isTagFor (self)
                        self.atup (nao ('hasTag'), tag.uri)

class TTLDoc:

        prefixes = \
"""
@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .
@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .
@prefix dc: <http://purl.org/dc/elements/1.1/> .
@prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> .
@prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> .
@prefix nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> .
@prefix nmm: <http://www.tracker-project.org/temp/nmm#> .
@prefix nao: <http://www.semanticdesktop.org/ontologies/2007/08/15/nao#> .
@prefix codethink: <http://www.codethink.co.uk/tracker/test/uri#> .
"""

        def __init__ (self):
                self.subjects = {}
                self.ordered = []

        def addtuple (self, tup):
                subject, predicate, object = tup
                if subject in self.subjects:
                        self.subjects[subject] += [tup]
                else:
                        self.subjects[subject] = [tup]
                        self.ordered.append (subject)

        def addresource (self, rdfresource):
                for tup in rdfresource.tups:
                        self.addtuple (tup)

        def addresources (self, resources):
                [self.addresource (res) for res in resources]

        def ttl (self):
                ttlstr = self.prefixes + '\n\n'

                def cstr (pred, obj):
                        return '\t' + ' '.join ([pred, str(obj)]) + ' ;\n'
                def estr (pred, obj):
                        return '\t' + ' '.join ([pred, str(obj)]) + ' .\n\n'

                for subject in self.ordered:
                        subjstr = subject + '\n'
                        tuples = self.subjects[subject]
                        for subject, predicate, object in tuples [0:-1]:
                                subjstr += cstr (predicate, object)
                        subject, predicate, object = tuples [-1]
                        subjstr += estr (predicate, object)

                        ttlstr += subjstr

                return ttlstr

def main (argv):
        doc = TTLDoc()

        space = NIE(uri='codethink:gallery-test-graph-001')
        tag = NAOTag(label='TEST', of=space)

        photos = [NMMPhoto(of=space) for i in range (0, 6000)]
        taggedphotos = [NMMPhoto(tag=tag, of=space) for i in range (0, 4000)]
        videos = [NMMVideo(of=space) for i in range (0, 3500)]
        taggedvideos = [NMMVideo(tag=tag, of=space) for i in range (0, 1500)]

        doc.addresource  (space)
        doc.addresource  (tag)
        doc.addresources (photos)
        doc.addresources (taggedphotos)
        doc.addresources (videos)
        doc.addresources (taggedvideos)

        fname = argv[1]
        f = open (fname, 'w')
        f.write (doc.ttl())

import sys

if __name__ == "__main__":
        sys.exit (main (sys.argv))
