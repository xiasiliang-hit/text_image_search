import urllib2
import io
import pdb
import time

idfilename = "ASIN"
imagefilename = "IMAGE"
errorfilename = "url_error.txt"


image_repo_path = "./image/"

waittime = 5

def download_images():
    fid = open(idfilename)
    furl = open(imagefilename)

    ids = [line for line in fid.readlines()]
    url_lines = [line for line in furl.readlines()]

#    pdb.set_trace()


    fail_url = 0;
    fail_id = 0;
    for i in range(0, len(ids)):
        id = ids[i][:-1]
        urls = url_lines[i].split(' ')
        urls = urls[1:]
        num = urls[0]

        print "id: " + id
        
        open(errorfilename, 'w').close()

        id_flag = True
        for url in urls:
            if  url[-1] == '\n':
                url = url[:-1]

            print "URL: " + url
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
                fout = open(image_repo_path + id + "_" + str(j) + ".jpg", "wb")
                fout.write(image)
                fout.close()

            else:
                print "fail:" + id
                print url
                fout = open(errorfilename, "a")
                fout.write(id + "\n")
                fout.write(url + "\n")

                fail_url += 1
                id_flag = False

            if id_flag == False:
                fail_id += 1

    print "failed id count: " + str(fail_id)
    print "failed url count: " + str(fail_url)
        
def main():
    download_images()
            

if __name__ == "__main__":
    main()
