from termcolor import colored


vacant =set("fgh")
ordering=list("fwgxhyizj")#our affected region
q="gj,hj,fi".split(",")



fillindex=frontindex=0
nodetoinsert, insertafter=q.pop()
frontnode=None

def printstate():
    print()
    print(f"nodetoinsert={nodetoinsert}, insertafter={insertafter}")# frontnode={frontnode}")
    orderingc=[]
    for i,x in enumerate(ordering+[" "]):

        if x in vacant:
            x=colored(x,"blue")
        if fillindex>i:
            x=colored(x,None,"on_green")
        if fillindex<=i<frontindex:
            x=colored(x,None,"on_dark_grey")
        if frontindex==i:
            x=colored(x,None,"on_magenta")
        orderingc.append(x)
    

    print("".join(orderingc))
    indexstr=[" "]*(len(ordering)+1)
    indexstr[frontindex]="^"
    indexstr[fillindex]="^"
    print("".join(indexstr))


for frontindex in range(len(ordering)):
    
    frontnode=ordering[frontindex]
    printstate()
    

    if not frontnode in vacant:
        ordering[fillindex]=frontnode
        fillindex+=1
        print("shifting",frontnode)
    else:
        print("ignoring",frontnode)
        vacant.remove(frontnode)

    

    while(frontnode==insertafter):
        printstate()
        ordering[fillindex]=nodetoinsert
        fillindex+=1
        
        print("inserting",nodetoinsert) 
        
        if not q:
            break
        nodetoinsert, insertafter=q.pop()

nodetoinsert, insertafter=None,None
frontnode=None
frontindex+=1
printstate()

