import cv2
import numpy as np
import os
import sys

L=['0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z']



def feature(image):
    img=cv2.imread(image)
    gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    gray2=cv2.resize(gray,(20,40))
    retval,result=cv2.threshold(gray2,75,255,cv2.THRESH_BINARY|cv2.THRESH_OTSU)
    i=j=0
    x=gray2.shape[0]
    y=gray2.shape[1]
    f=[]

    while i<x:
        j=0
        while j<y:
            ans=0
            for m in range(4):
                for n in range(4):
                    if result[i+m,j+n]==255:
                        ans+=1
            f+=[float(ans)/16]
            j+=4
        i+=4
    return f

def predict(path,svm):
    f=feature(path)
    fs=np.array(f,dtype=np.float32)
    index=int(svm.predict(fs))
    return L[index]

def lpr(path):
    # svm=cv2.ml.SVM_create()
    svm = cv2.SVM()
    # print help(svm)
    svm.load('../LPR.xml')
    if not path:
        path=sys.argv[1]
    im_list=os.listdir(path)
    ans={}
    for im in im_list:
        name=os.path.join(path,im)
        s=predict(name,svm)
        ans[im]=s
    # for k,v in sorted(ans.items()):
    #     print k,v

        
    return ans

# lpr()

