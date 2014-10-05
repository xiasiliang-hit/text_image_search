import nltk
from nltk.corpus import stopwords

import os
import cPickle as pkl
import sys
import codecs
import pdb


debug = False


prefix = "./small/"


titlefilename = prefix + "TITLE"
dictfilename = prefix + "freq.txt"
testfilename = prefix + "test.txt"

dict_binfilename = prefix + "dict.bin"
title_binfilename = prefix + "title.bin"
word_binfilename = prefix + "word.bin"

reverse_index_binfilename = prefix + "index.bin"


# titlefilename = "TITLE"
# dictfilename = "dict1.txt"
# testfilename = "test1.txt"

# dict_binfilename = "dict1.bin"
# title_binfilename = "title1.bin"
# word_binfilename = "word1.bin"

# reverse_index_binfilename = "reverse_index1.bin"


word_global  = []
title_global = []

dict_global = {}

       
#get rid of special characters
def preprocess(str):

    local_str = ""
    for s in str:
#        if ord(s) >= 0 and ord(s) <= 127:
#            local_str += s

        if (s >= 'a' and s <= 'z') or (s >= 'A' and s <= 'Z'):
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
        
#    words = []

    #tokenize, to tokens
    tokens = nltk.word_tokenize(local_str)

#    for token in tokens:
#        words.append(token)
    
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
        
#    pdb.set_trace()    

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
#    pdb.set_trace()

    i  = 0
    for line in lines:
        i += 1
        print i

        str_all += line

        #temp = preprocess(line)
        #word_list = word_list + temp

    #    title_global.append(line.replace("\n", ""))     #no effect for tokenize    
    word_list = preprocess(str_all)
    dict_word = make_freqency_vector(word_list)



    if debug:
        pdb.set_trace()
    
        # if i == 1:

        #     print line
        #     pdb.set_trace()
        
        # for word in dict_word.key():
        #     if word in dict_global:
        #         dict_global[word] += 1
        #     else:
        #         dict_global[word] = 1
    
    # try:
    #     dict_word = make_freqency_vector(str_all)
    # except UnicodeDecodeError:
    #     #    print str_all
    #     break


    pkl.dump(dict_word, open(dict_binfilename, "wb"))

    
    fout = open(dictfilename, "w")    
    for key in sorted(dict_word.keys()):
        fout.write(key + " " + str(dict_word[key]) + "\n")

        # alread sorted
        if (key == '11-c' and debug):
            pdb.set_trace()


        #add to word list by the way!!!
        word_global.append(key)


    #pdb.set_trace()
        
        # print(key, file = "dict.txt") 
        # print (dict_word[key]) >> "dict.txt"
    fout.close()
    pkl.dump(word_global, open(word_binfilename, "wb"))
        

    
#return list
def build_candidate_vector(filename):

    print "f: build_candidate_vector"
    
    f = open(filename)
    lines = f.readlines()
    f.close()


    title_list = []
    for line in lines:
        title_list.append(line)


    vector_list = []
    for title in title_list:
        vector_dict = make_freqency_vector(title)
        vector_list.append(vector_dict)

    return vector_list



def rank_bagofwords(title, filename):
    print "begin rank_bagofwords()"

    target_vector = make_freqency_vector(title)

    candidate_vectors = build_candidate_vector(filename)

    distance_list = []
    
    for vector in candidate_vectors:
        distance = 0
        
        for word in vector.keys():
            if word in target_vector.keys():
                a = vector[word]
                b = target_vector[word]
                                
            else:
                a = vector[word]
                b = 0

            distance += (a - b) * (a - b)

        for word in target_vector.keys():
            if word in vector.keys():
                #already calculatd in previous for loop
                a = 0
                b = 0
            else:
                a = 0
                b = target_vector[word]

            distance += (a - b) * (a - b)

        distance_list.append(distance)

    sorted_list = sorted(distance_list)
        
    return (distance_list, sorted_list)

def evaluate(filename):
    print "begin evaluate()"

    title_list = []
    f = open (filename)
    lines = f.readlines()
    for line in lines:
        title_list.append(line)

    total = len(title_list)


    print "loading finished"
    for index in range(0, len(title_list)):
        
        title = title_list[index]
        distance_list, sorted_list = rank_bagofwords(title, filename)
        
        score = distance_list[index]
        rank = sorted_list.index(score) + 1

        rate = rank / total

        print "=="
        print index
        print rate



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



def build_index():

    print "build_index"

    # f = open(dictfilename)
    # lines = f.readlines()
    # f.close()
    
    
    # word_list = []
    # for line in lines:
    #     word = line.split(' ')[0]
    #     fre = line.split(' ')[1]
    
#    word_list.append(word)

    title_global = pkl.load(open(title_binfilename))
    word_global = read_word_bin()
    index_result = [None] * len(word_global)
    

#    i = 0
    for i in range(0, len(title_global)):
        title = title_global[i]
        
        #i += 1
#        print title
        print i

#        if (i == 2):
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

#            if (pos == 2):
#                pdb.set_trace()

#            i_title = title_global.index(title)
            if index_result[pos] is not None:
                index_result[pos].append(i)
            else:
                title_list = []
                title_list.append(i)
                index_result[pos] = title_list

#        pdb.set_trace()
    print "write bin"
    
    #pkl.dump(word_global, open("word_global.bin", "wb"))
    #pkl.dump(title_global, open("title_global.bin", "wb"))
    pkl.dump(index_result, open(reverse_index_binfilename, "wb"))

#    pdb.set_trace()
    
def read_dict():
    return pkl.load(open(dict_binfilename))

    
if __name__ == "__main__":
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

    
    build_dict(titlefilename)
    write_title_bin()

    
    build_index()

#    l = read_title_bin()

#    pdb.set_trace()
    

    # build_index()

    # w = pkl.load("word_global.bin", "wb")
    # print w

    # ind = pkl.load("dict_global.bin", "wb")
    # print ind
#    mkdict(sys.argv[1])
#    test()
