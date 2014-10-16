import cv2
import numpy as np
import os
import pdb

currentDir = "./2/"
vecdir = "./2vec/"


def all_save_sift_vector():
    files  = os.listdir(currentDir)
    for f in files:        
        if f.find(".jpeg"):
#            pdb.set_trace()
            #filename = currentDir + f
            save_sift_vector(f)
        
def save_sift_vector(filename):
    #filename = "airlane.jpeg";    
    img = cv2.imread(currentDir + filename)
#gray= cv2.cvtColor(img)
#sift = cv2.SIFT()

    detector = cv2.FeatureDetector_create("SIFT")
    descriptor = cv2.DescriptorExtractor_create("SIFT")

    skp = detector.detect(img)
    skp, mat = descriptor.compute(img, skp)

#tkp = detector.detect(template)
#tkp, td = descriptor.compute(template, tkp)

#kp = sift.detect(img)
#print len(kp)

    print len(mat)
    print len(mat[0])
    
    fout = open(vecdir + filename + ".vec", "w")

    for row in mat:
        for dim in row:
            fout.write(str(dim) + " ")
        fout.write("\n")
    fout.close()

    
def main():
    os.mkdir(currentDir)
    os.mkdir(vecdir)
    
    all_save_sift_vector()
    
if __name__ == '__main__':
    main()
    
#for vec in sd[0]:
#    for dem in vec:
#        print dem

#for vec in sd[0]:
#    print sd

#for point in kp:
#    for dem in point:
#        print point

#img_kp=cv2.drawKeypoints(img,kp)
#cv2.imwrite('sift_keypoints.jpg',img_kp)
