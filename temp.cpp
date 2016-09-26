

int main(){

	ClassName* obj_ptr;
	stack<ClassName*> mystack;
	
	mystack.push(initial_obj_ptr);
	
	while(1){
		obj_ptr = mystack.top();
		mystack.pop();
	
		ClassName* new_obj_ptr = new ClassName;
	
		new_obj_ptr->explore_from(obj_ptr);
	
		mystack.push(new_obj_ptr);
	
	}
	
	return 0;
}