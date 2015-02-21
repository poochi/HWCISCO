from random import randint
from random import sample
MAX_TOWERS=10
towercnt = randint(2,MAX_TOWERS);
freq=[1,2,3,4,5,6];
out = open("towers.txt","w");
for i in range(towercnt):
    x='A'+str(i+1)+':'
    out.write('{}'.format(x));
    temp = sample(freq,randint(1,len(freq)));
    for each in temp[:-1]:
        x='f'+str(each)+','
        out.write('{}'.format(x))
    x='f'+str(temp[-1])
    out.write('{}'.format(x))
    out.write('\n')
out.close()
