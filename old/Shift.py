
nodes=[*"fwgxhyizj"]

nextnodes=dict()
nextnodes["x"]=["y"]
nextnodes["i"]=["j"]

newedgeslist=[("i","f"),("h","g"),("j","h")]
#newedges=dict(newedges)
newedgess=dict()
for s,e in newedgeslist:
    newedgess.setdefault(s,[]).append(e)

Q=[]
frontiers=dict()
for n in reversed(nodes):
    if n in newedgess:
        if n in frontiers:
            for end in newedgess[n]:
                frontiers[end]=frontiers[n]
        else:
            for end in newedgess[n]:
                print(end,frontiers.get(end,nodes[0]),n,frontiers)
                frontiers[end]=max(frontiers.get(end,nodes[0]),n,key=nodes.index)
            
print(frontiers)

print(newedgess)

#Q=sorted(newedges,key=lambda edge:nodes.index(edge[0]))
print(nodes)
def findfollowingnodes(n):
    ret={}
    s=[n]
    while s:
        head=s.pop()
        if head in ret:
            continue
        s.extend(nextnodes.get(head,[]))
        ret.add(head)
    return ret

Q=sorted([(n,frontiers[n])for end,n in newedgeslist],key=lambda sf:(-nodes.index(sf[1]),nodes.index(sf[0])))

vacant=