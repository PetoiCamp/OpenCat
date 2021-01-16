import unittest
from ardSerial import wrapper

class TestSum(unittest.TestCase):
    def test_scheduler(self):
        schedule = [['ktr',1000],\
                    ['d',1500],\
                    ['c',500],\
                    ['d',150],\
                    ['c0',5],\
                    ['d',15],\
                    ['c2',5],\
                    ['d',15],\
                    ['c15',5],\
                    ['d',15],\
                    ['m',[1,-30],1],\
                    ['d',15],\
                    ['i',[5,50,7,100,1,0],1],\
                    ['d',15],\
                    ['l',[1,2,3,4,5,6,7,8,9,1,2,3,4,5,6],5],\
                    ['d',15],\
                    ['m',['m','5','55'],1],\
                    ['d',15],\
                    ['b',['b','10','255'],1],['d',15]]


        for task in schedule:
            wrapper(task)

        '''
        sol_str=['ktr','d','c','c0','c2','c15',\
                 ['m',[1,-30]],\
                 ['i',[5,50,7,100,1,0]],\
                 ['l',[1,2,3,4,5,6,7,8,9,1,2,3,4,5,6]],\
                 ['m','5','55'],\
                 ['b','10','255'],'d']

        #A for loop here would have made it diffcult to trace the errors
        
        self.assertEqual(wrapper(schedule[0]),sol_str[0])
        self.assertEqual(wrapper(schedule[1]),sol_str[1])
        self.assertEqual(wrapper(schedule[2]),sol_str[2])
        self.assertEqual(wrapper(schedule[3]),sol_str[3])
        self.assertEqual(wrapper(schedule[4]),sol_str[4])
        self.assertEqual(wrapper(schedule[5]),sol_str[5])
        self.assertEqual(wrapper(schedule[6]),sol_str[6])
        self.assertEqual(wrapper(schedule[7]),sol_str[7])
        self.assertEqual(wrapper(schedule[8]),sol_str[8])
        self.assertEqual(wrapper(schedule[9]),sol_str[9])
        self.assertEqual(wrapper(schedule[10]),sol_str[10])
        self.assertEqual(wrapper(schedule[11]),sol_str[11])

        schedule = [['m',[1,45],1],\
                    ['d',5],\
                    ['m',[1,-30],1],\
                    ['b',['b','10','255'],1],\
                    ['ktr',10]]
        sol_str=[['m',[1,45]],\
                 'd',\
                 ['m',[1,-30]],\
                 ['b','10','255'],\
                 'ktr']
        #A for loop here would have made it diffcult to trace the errors
        self.assertEqual(wrapper(schedule[0]),sol_str[0])
        self.assertEqual(wrapper(schedule[1]),sol_str[1])
        self.assertEqual(wrapper(schedule[2]),sol_str[2])
        self.assertEqual(wrapper(schedule[3]),sol_str[3])
        self.assertEqual(wrapper(schedule[4]),sol_str[4])
 #       assert pro.wrapper(schedule[5])==
        '''

if __name__ == '__main__':
    unittest.main()
