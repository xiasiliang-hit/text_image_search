#build wordlist
#build wordlist with frequency

#generate three bin files and freq.txt

import nltk
from nltk.corpus import stopwords

import os
import cPickle as pkl
import sys
import codecs
import pdb
from multiprocessing import Process, Pipe
import threading
from threading import Thread

debug = False
thread_n = 16


prefix = "./multi/"


titlefilename = prefix + "TITLE"
dictfilename = prefix + "freq.txt"
testfilename = prefix + "test.txt"

dict_binfilename = prefix + "dict.bin"  #dictionary with frequency 
title_binfilename = prefix + "title.bin"  
word_binfilename = prefix + "word.bin"  #key word list sorted

reverse_index_binfilename = prefix + "index.bin"

#word_global  = []
#title_global = []

dict_global = {}

       
#get rid of special characters
def preprocess(str):

    local_str = ""
    for s in str:

        if (s >= 'a' and s <= 'z') or (s >= 'A' and s <= 'Z') :
            local_str += s
        else:
            local_str += ' '
            
        # if (s == ' ' or s == '\t'):
        #     final_str += s
        
        
        # if (s >= 'a' and s <= 'z'):
        #     final_str += s
        
        

        # if (s >= 'A' and s <= 'Z'):
        #     final_str += s
        
        
        # if (s >= '0' and s <= '9'):
        #     final_str += s
        

    #tokenize, to tokens
    tokens = nltk.word_tokenize(local_str)

    
    #normalization: Book to book
    words_norm = [word.lower() for word in tokens]    
    words_norm = [word for word in words_norm if word not in stopwords.words("english")]
    
    porter = nltk.PorterStemmer()
#    for sent in sents:    
#        words_norm = [word.lower() for word in sent]

    #stemming: books to book
    stemmed_words = []
    for word in words_norm:
        try:
            w = porter.stem(word)
        except UnicodeDecodeError:
            print w
        
        stemmed_words.append(w)
        
    #return a word list
    return stemmed_words



def delete_stopwords(str):
    return


    
def make_freqency_vector(word_list):

#    print "f: make_freqency_vector"

    dict_word = {}
#    stemmed_words = [porter.stem(word) for word in words_norm]
    for word in word_list:
        if word in dict_global:
            dict_word[word] += 1
        else:
            dict_word[word] = 1

    return dict_word

def build_dict(filename):
    print "f: build_dict"

    f = open(filename)
    lines = f.readlines()
    f.close()

    str_all = ""
    word_list = []

    i  = 0
    for line in lines:

        i += 1
        #print i
        
        str_all += line

    word_list = preprocess(str_all)
    dict_word = make_freqency_vector(word_list)

    if debug:
        pdb.set_trace()

    #wirte to bin file, dictionary with frequency
    pkl.dump(dict_word, open(dict_binfilename, "wb"))

    fout = open(dictfilename, "w")    
    for key in sorted(dict_word.keys()):
        fout.write(key + " " + str(dict_word[key]) + "\n")

        #add to word list by the way!
        word_global.append(key)


    #pdb.set_trace()
        
        # print(key, file = "dict.txt") 
        # print (dict_word[key]) >> "dict.txt"
    fout.close()
    pkl.dump(word_global, open(word_binfilename, "wb"))
        

def write_title_bin():

    title_list = []
    f = open(titlefilename)
    for line in f:
        title_list.append(line.replace("\n", ""))
    f.close()

    pkl.dump(title_list, open(title_binfilename, "wb"))

def read_title_bin():
    return pkl.load(open(title_binfilename))

def read_word_bin():
    return pkl.load(open(word_binfilename))

       
def test():
    # str = ["book", "books", "Book", "BOOK", "classification", "classify", "classifier"]
    # str2 = "classifier classify classification"

    # porter = nltk.PorterStemmer()
    # word_norm = [word.lower() for word in str]

    # print word_norm
    # stemmed_words = [porter.stem(word) for word in word_norm]

    # #stemmed_words2 = porter.stem(str2)
    # print (stemmed_words)

    f = open("TITLE")
    lines = f.readlines()
    i = 0
    
    result_list = []
    for line in lines:
#        x = line.encode("utf-8")
#        y = line.decode(encoding = "ascii")
        #print x

        #line = preprocess(line)
        result_list.append(line)
    
    pkl.dump(result_list, open("result_list.bin", "wb"))

#    l = pkl.load(open("result_list.bin"))

#    for i in l:
#        print l
    
#        print l[0]

        
        # try:
        # #    print i
        #     i += 1
        #     w = nltk.word_tokenize(line)

        # except UnicodeDecodeError:

        #     print line
            #print 
            #            print w
            
            #break
        
        #print line.encode("utf-8", "strict")
#        print line.encode("ascii").decode("ascii")
    
    return



def build_index(start, end, l_return):
    #pdb.set_trace()
    print "build_index"

    #global word_global
    #global title_global
    
#    title_global = read_title_bin()
    #title_piece = title_global[start:end]  ##only a loci for processing

#    word_global = read_word_bin()
    
    index_result = [None] * len(word_global)
    #init so that it is [] instead of None
    for i in range(0, len(word_global)):
        index_result[i] = []

#    i = 0
    for i in range(start, end):
        title = title_global[i]
        
#        print title
        print i

#        if (i == 1434):
#            pdb.set_trace()
        
        
        word_list = preprocess(title)
        
        for word in word_list:

            if (debug == True):
                pdb.set_trace()

            #if word in word_global:
            try:   
                pos = word_global.index(word)
            
            except ValueError:
                print "error:" + word + "word_global" 
                continue

            if index_result[pos] is not None:
                index_result[pos].append(i)
            else:
                title_list = []
                title_list.append(i)
                index_result[pos] = title_list

    #print "write bin"
    
    #pkl.dump(word_global, open("word_global.bin", "wb"))
    #pkl.dump(title_global, open("title_global.bin", "wb"))

#have not been merged 
#    pkl.dump(index_result, open(reverse_index_binfilename, "wb"))

    for it in index_result:
        l_return.append(it)

    #return index_result


#not used here
#used in next step in seasrch.py    
def read_dict():
    return pkl.load(open(dict_binfilename))


def test1():

    print "test1"
    title_global.append("test1")
#l = []
        #l = r
    #return r

def test2():

    print "test2"
    print len(title_global)

def multi_index(start, end, conn):
    
    l = build_index(start, end)

#    l = []
#    test1(start,end, l)

#    print l
    conn.send(l)

    return 



class ThreadTitlePartition(threading.Thread):

    return_list = []
    def __init__(self, st, end):
        threading.Thread.__init__(self)
        self.st = st
        self.end = end
        return
    
    def run(self):
        self.return_list = build_index(self.st, self.end)

        return


#     def build_index(start, end):
#     #pdb.set_trace()
#         print "build_index"
    
# #        title_global = read_title_bin()
#     #title_piece = title_global[start:end]  ##only a loci for processing

# #        word_global = read_word_bin()
#         index_result = [None] * len(word_global)

#     #init so that it is [] instead of None
#         for i in range(0, len(word_global)):
#             index_result[i] = []

# #    i = 0
#         for i in range(start, end):
#             title = title_global[i]
        
#         #i += 1
# #        print title
#             print i

# #        if (i == 1434):
# #            pdb.set_trace()
        
        
#             word_list = preprocess(title)
        
#             for word in word_list:

#                 if (debug == True):
#                     pdb.set_trace()

#             #if word in word_global:
#                 try:   
#                     pos = word_global.index(word)
            
#                 except ValueError:
#                     print "error:" + word + "word_global" 
#                     continue

#                 if index_result[pos] is not None:
#                     index_result[pos].append(i)
#                 else:
#                     title_list = []
#                     title_list.append(i)
#                     index_result[pos] = title_list
#         return index_result


def index_multi_threads():

    global word_global
    global title_global
    
    title_global = read_title_bin()
    word_global = read_word_bin()
    
    l_result = []
    t_pool = []
    
    for i in range(0, thread_n):
        start = int(i * len(title_global) / thread_n)
        end = int((i+1) * len(title_global) / thread_n)

        l = []
        t = Thread(target = build_index, args = (start, end, l, ))
        t_pool.append(t)
        l_result.append(l)
        
        t.start()
        #t.join()
        
    for th in t_pool:
        th.join()
        
    if True:
        t =0
        ind = 0
        for i in range(0, thread_n):
            print l_result[i][ind]

    #pdb.set_trace()
        
    final_index = [None]* len(word_global)
    
    #merge
    for j in range(0, len(word_global)):
        merged = []
        for k in range(0, thread_n):
            merged = merged + l_result[k][j]
            
        final_index[j] = merged

    #pdb.set_trace()

    print "write index bin"
    pkl.dump(final_index, open(reverse_index_binfilename, "wb"))




if __name__ == "__main__":
    index_multi_threads()

    #    parse()
    
#    str = readfile(sys.argv[1])
#    ie_process(str)



#    evaluate(sys.argv[1])    
#    test()
#    dict_test = build_candidate_vector(sys.argv[1])
    
#     for vector in dict_test:

#         print (vector)
#         #print dict_test[key]

#     print len(dict_test)
# #   





    
#    
#    build_dict(titlefilename)
#    write_title_bin()




    
#     title_global = read_title_bin()
#     word_global = read_word_bin()

    
#     l_result = []

#     p_pool = []
#     l_parent_conn = []
    
#     for i in range(0, thread_n):
#         start = int(i * len(title_global) / thread_n)
#         end = int((i+1) * len(title_global) / thread_n)
        
# #        start = i *5
# #        end = (i+1) *5
# #        pdb.set_trace()
#         parent_conn, child_conn = Pipe()
#         p = Process(target = multi_index, args = (start, end, child_conn, ))
#         p_pool.append(p)
#         l_parent_conn.append(parent_conn)
        
#         p.start()
        
#     for p in p_pool:
#         p.join()

        
#     for i in range(0, thread_n):
#         parent_conn = l_parent_conn[i]
#         l = parent_conn.recv()
#         l_result.append(l)

        

#     if True:
#         t =0
#         ind = 0
#         for i in range(0, thread_n):
#             print l_result[i][ind]

# #    pdb.set_trace()
        
#     final_index = [None]* len(word_global)
    
#     #merge
#     for j in range(0, len(word_global)):
#         merged = []
#         for k in range(0, thread_n):
#             merged = merged + l_result[k][j]
            
#         final_index[j] = merged
#         #pdb.set_trace()

#     print "write index bin"
#     pkl.dump(final_index, open(reverse_index_binfilename, "wb"))
    
        








    #    pdb.set_trace()
    
    
    
#    build_index()

#    l = read_title_bin()

#    pdb.set_trace()
    

    # build_index()

    # w = pkl.load("word_global.bin", "wb")
    # print w

    # ind = pkl.load("dict_global.bin", "wb")
    # print ind
#    mkdict(sys.argv[1])
#    test()



