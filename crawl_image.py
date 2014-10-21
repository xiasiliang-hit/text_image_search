import urllib2
import io
import pdb
import time
import sys
import atexit

idfilename = "ASIN"
imagefilename = "IMAGE"
errorfilename = "url_error.txt"


waittime = 5

baseupdate = 0


def conti(*args):
    recordpath = "./record/"
    f = open(recordpath + str(args[0]) + ".n", "w")
    f.write(str(baseupdate))

    pdb.set_trace()
    
    print "write base_update"
    f.close()
    

def download_images(n):

    recordpath = "./record/"

    baseupdate = 0
    try:
        fr = open(recordpath + str(n) + ".n", "r")
        lines = fr.readlines()
        baseupdate = int(lines[0])
        fr.close()
    except:
        print "no record file, maybe the first time"
        
    
    pre = "./" + str(n)

    image_repo_path = pre + "/image/"


    fid = open(idfilename)
    furl = open(imagefilename)

    ids = [line for line in fid.readlines()]
    url_lines = [line for line in furl.readlines()]

    
    be = 500000 * int(n)
    en = be + 500000

    add = 0 + baseupdate
    ids = ids[be+add : en]
    url_lines = url_lines[be+add : en]
    
#    pdb.set_trace()


    fail_url = 0
    fail_id = 0
    line_num = 0
    fw = open(pre+"name.txt", "w")
    open(pre + errorfilename, 'w').close()
    
    for i in range(0, len(ids)):
        #pdb.set_trace()
        my_id = ids[i][:-1]
        urls = url_lines[i].split(' ')
        urls = urls[1:]
        num = urls[0]

#        print "id: " + my_id
#        print "line: " + str(line_num+ add)
        baseupdate = line_num + add
        line_num += 1

        f = open(recordpath + n + ".n", "w")
        f.write(str(baseupdate))

#        pdb.set_trace()
    
        print str(n) + ":" + str(baseupdate)
        f.close()
        

        id_flag = True
        count = 0
        for url in urls:
            if  url[-1] == '\n':
                url = url[:-1]

#            print "URL: " + url
            flag = False
            for j in range(0, 5):
                try:
                    image = urllib2.urlopen(url).read()
                    flag = True
                    break
                except:
                    print "exception:" + url
                    flag = False
                    time.sleep(waittime)

            if flag:
                filename = my_id + "_" + str(count) + ".jpg"
                fw.write(filename + "\n")
                
                fout = open(image_repo_path + filename, "wb")
                fout.write(image)
                fout.close()
                count += 1
            else:
                print "fail:" + my_id
                print url
                fout = open(errorfilename, "a")
                fout.write(my_id + "\n")
                fout.write(url + "\n")

                fail_url += 1
                id_flag = False

            if id_flag == False:
                fail_id += 1

    print "failed id count: " + str(fail_id)
    print "failed url count: " + str(fail_url)
    fw.close()

def main():

    cur = sys.argv[1]

#    atexit.register(conti, cur)
    download_images(cur)
            

if __name__ == "__main__":
    main()
