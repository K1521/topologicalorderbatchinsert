import random

def makegraph(numnodes=1000,connspernode=1):
    nodes=[str(x) for x in range(numnodes)]
    random.shuffle(nodes)
    connections=[]
    for i in range(connspernode*numnodes):
        a,b=random.randint(0,numnodes-1),random.randint(0,numnodes-1)
        if a==b:
            continue
        if a<b:
            a,b=b,a
        #a>b
        connections.append((nodes[a],nodes[b]))
    connections=list(set(connections))
    random.shuffle(connections)
    random.shuffle(nodes)
    return nodes,connections
