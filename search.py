import mkdict as mk
import cPickle as pkl
import sys
import pdb
import operator


debug = False

prefix = "./small/"


dictfilename = prefix + "dict.txt"

dict_binfilename = prefix + "dict.bin"
title_binfilename = prefix + "title.bin"
word_binfilename = prefix + "word.bin"
reverse_index_binfilename = prefix + "index.bin"


wordfilename_w = prefix + "mapper.txt"
reverse_index_filename_w = prefix + "index.txt"
dictfilename_w = prefix + "freq.txt"


return_length = 10


class TextSearch:

    reverse_index = []
    word_global = []
    dict_global = {}
    title_global = []

    weight = []

    def load_bin(self):
        self.reverse_index = pkl.load(open(reverse_index_binfilename))
        self.word_global = pkl.load(open(word_binfilename))
        self.dict_global = pkl.load(open(dict_binfilename))

        self.title_global = pkl.load(open(title_binfilename))

        self.generateWeight()

        if debug:
            pdb.set_trace()
        

    def search(self, keywords):
    
        #pdb.set_trace()
        processed = mk.preprocess(keywords)
        grade = {}

        if debug:
            pdb.set_trace()
        
        for word in processed:
            try:
                pos = self.word_global.index(word)
        
            except ValueError:

                if debug:
                    print "key word not exist:" + word
                continue

            containing = self.reverse_index[pos]
        #pdb.set_trace()

            for title_index in containing:
                if title_index in grade.keys():
                    grade[title_index] += 1
                #grade[num] += weight[pos]
                else:
                    grade[title_index] = 1
                #grade[num] = wieght[pos]


#                pdb.set_trace()
            
        sorted_grade = sorted(grade.items(), key = operator.itemgetter(1), reverse=True)

        
        #for j in range(0, return_length):
        for (i, g) in sorted_grade:
            print self.title_global[i]
            print g

#        pdb.set_trace()
    def generateWeight(self):
        for i in range(0, len(self.word_global)):
            self.weight.append(1)


    def write_text(self):
        f = open(wordfilename_w, "w")
        for word in self.word_global:            
            f.write(word + "\n")
        f.close()

        
#        pdb.set_trace()

#        f = open(dictfilename_w, "a")
#        for word, fre in self.dict_global:            
#            f.write(word + ' ' + str(fre) + "\n")
#        f.close()

        f = open(reverse_index_filename_w, "w")
        for li in self.reverse_index:

            li_str = [str(num) for num in li]
            
            items = " ".join(li_str)
            f.write(items + "\n")
        f.close()

        
        
        
if __name__ == "__main__":

    engine = TextSearch()
    engine.load_bin()    
    engine.write_text()
    
	
    while (True):
        inp = raw_input("keywords:")
        engine.search(inp)
    
