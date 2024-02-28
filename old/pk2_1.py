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


nametoptr={name:i for i,name in enumerate("arbsctduevfwgxhyizj")}
#node->isvacant,onStack,childrenindicees
Node=namedtuple('Node', 'vacant,onStack,children,ord')
nodes=[Node(False,False,[],i) for i in range(len(nametoptr))]#replace durch pointer dereferenzierung
ordinv=list(range(len(nametoptr)))
connections="au,eg,xy,ij".split(",")
for start,end in connections:
    nodes[nametoptr[start]].children.append(nametoptr[end])

def ord(x,newval=None):
    r=nodes[x].ord
    if newval is not None:
        nodes[x].ord=newval
    return r
def vacant(x,newval=None):
    r=nodes[x].vacant
    if newval is not None:
        nodes[x].vacant=newval
    return r
def onStack(x,newval=None):
    r=nodes[x].onStack
    if newval is not None:
        nodes[x].onStack=newval
    return r
newconnections="jh,if,hg,ed,cb,ba".split(",")
newconnectionsptr=[(nametoptr[s],nametoptr[e])for s,e in newconnections]
newconnectionsptr.sort(key=lambda pse:ord(pse[1]))

print(newconnectionsptr)

Bs=[]
act=[]
endwindow=ord(newconnectionsptr[0][1])
for right,left in newconnectionsptr:
    if ord(left)<=endwindow:
        act.append((right,left))
    else:
        Bs.append(act)
        act=[(right,left)]

    endwindow=max(ord(right),endwindow)
Bs.append(act)

print(Bs)


def discover(B):
    B.sort(reverse=1,key=lambda lr:ord(lr[0]))#not sure if 0 or 1
    Q=[]
    def dfs(v,ub):
        vacant(v,True)
        onStack(v,True)
        for s in nodes[v].children:
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


Qs=[discover(B) for B in Bs]
print(Qs)