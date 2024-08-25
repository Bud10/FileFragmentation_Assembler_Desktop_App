#include "member_defn.cpp"

int main(){
	Index x,y;
	int ch;
	cout<<"\nChoose the option : "<<endl;
	cout<<"\n1.Fragment file \n2.Defragment file \n -->>"<<endl;
	cin>>ch;
	if (ch == 1)
		x.select_component(0);	
	else if (ch == 2)
		x.select_component(1);
	else 
		cout<<"\nInvalid choice"<<endl;
	return 0;
}