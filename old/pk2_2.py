from collections import namedtuple

#ord:node->index
#ord-1:index->node
#vacant:node->isvacant
#allocate:(node,index)->None #place v at index i
#SHIFT:(i,y)->None
#DISCOVER:B->Q
#dfs
#onStack
#ADD_EDGE(B


class Node:
    def __init__(self,name,ord):
        self.name=name
        self.vacant=False
        self.onStack=False
        self.children=[]
        self.ord=ord
    def __str__(self):
        return self.name
    def __repr__(self):
        return self.name
    

nametonode={}
ordinv=[]
for i,name in enumerate("arbsctduevfwgxhyizj"):
    n=Node(name,i)
    nametonode[name]=n
    ordinv.append(n)
    #ordinv.append(nametonode[name]:=Node(name,i))#lol maby dont
#ordinv=[(nametonode[name]:=Node(name,i)) for i,name in enumerate("arbsctduevfwgxhyizj")]#lol maby dont
#node->isvacant,onStack,childrenindicees
#nodes=[Node(False,False,[],i) for i in range(len(nametoptr))]#replace durch pointer dereferenzierung

connections="au,eg,xy,ij".split(",")
for start,end in connections:
    nametonode[start].children.append(nametonode[end])

def ord(node,newval=None):
    r=node.ord
    if newval is not None:
        node.ord=newval
    return r
def vacant(node,newval=None):
    r=node.vacant
    if newval is not None:
        node.vacant=newval
    return r
def onStack(node,newval=None):
    r=node.onStack
    if newval is not None:
        node.onStack=newval
    return r




#Bs=[]
#act=[]
#endwindow=ord(newconnectionsptr[0][1])
#for right,left in newconnectionsptr:
#    if ord(left)<=endwindow:
#        act.append((right,left))
#    else:
#        Bs.append(act)
#        act=[(right,left)]
#
#    endwindow=max(ord(right),endwindow)
#Bs.append(act)

#print(Bs)


def discover(B):
    B.sort(reverse=1,key=lambda lr:ord(lr[0]))#not sure if 0 or 1
    Q=[]
    def dfs(v,ub):
        vacant(v,True)
        onStack(v,True)
        for s in v.children:
            if onStack(s):
                raise Exception("cycle")
            if not vacant(s) and ord(s)<ub:
                dfs(s,ub)
        onStack(v,False)
        Q.append((v,ordinv[ub]))


    for right,left in B:
        if not vacant(left):
            dfs(left,ord(right))
    return Q


#Qs=[discover(B) for B in Bs]
#print(Qs)
#print(discover(Bs[-1]))

#AddEDGE aber meine version
newconnections="jh,if,hg,ed,cb,ba".split(",")
for start,end in newconnections:#E=E U B
    nametonode[start].children.append(nametonode[end])
newconnectionsptr=[(nametonode[s],nametonode[e])for s,e in newconnections]


print(newconnectionsptr)
"""newconnectionsptr.sort(key=lambda pse:ord(pse[1]))
Bs=[]
act=[]
endwindow=ord(newconnectionsptr[0][1])#-1
startwindow=endwindow
for right,left in newconnectionsptr:
    if ord(left)<=endwindow:# or endwindow==-1:
        act.append((right,left))
    else:
        Bs.append((startwindow,act))
        act=[(right,left)]
        startwindow=ord(left)

    endwindow=max(ord(right),endwindow)
Bs.append((startwindow,act))"""

newconnectionsptr.sort(key=lambda pse:-ord(pse[1]))#version machen die nur lower/higher bound anguckt
Bs=[]
act=[]
lowerbound=ord(newconnectionsptr[0][0])#-1
for right,left in newconnectionsptr:
    if ord(right)>=lowerbound:# or endwindow==-1:
        act.append((right,left))
    else:
        Bs.append((lowerbound,act))
        act=[(right,left)]
        startwindow=ord(right)

    lowerbound=min(ord(left),lowerbound)
Bs.append((lowerbound,act))

print(Bs)

#Qs=[discover(B) for lb,B in Bs]
#print(Qs)


def Shift(i,Q):
    def allocate(v,i):
        oldnode=ordinv[i]
        oldord=ord(v,i)
        ordinv[i]=v


    n=0
    i0=i
    while Q:
        print(ordinv[i0:])
       
        w=ordinv[i]
        print(w,vacant(w),Q[-1],i,n)
        if vacant(w):
            n+=1
            vacant(w,False)
            #print("vacant")
        else:
            allocate(w, i - n)

        (v,d)=Q[-1]
        while Q and w==d:
            n-=1
            allocate(v, i - n)
            Q.pop()
            if Q:(v,d)=Q[-1]
        i+=1

for lb,B in Bs:
    Q=discover(B)
    print(Q)
    Shift(lb,Q)
#Qs=[discover(B) for lb,B in Bs]
print(ordinv)