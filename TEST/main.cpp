#include <iostream>
#include <forward_list>
  using namespace std;

int main()
{

    int counter = 0 ;
    forward_list <int> data_list ;
    bool first_pass = true;
    auto it = data_list.before_begin();
    auto it2 = data_list.end();
    while(1)
    {
      for(int i =0; i < 10; i++)
      {
        data_list.emplace_after(it,counter);
        counter++;
        it++;
      }

        while(!data_list.empty())
        {
          cout << data_list.front() << endl;
          data_list.pop_front();
        }
    }

}
