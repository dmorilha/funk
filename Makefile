funk: funk.c
	@gcc funk.c -o funk

test: funk
	@echo "YUI.add('test', function (Y, NAME) { console.log(NAME); }, '0.1.0', { requires: ['base', 'oop']})" | ./funk YUI.add 1;
	@echo "YUI.add('test', function (Y, NAME) { console.log(NAME); }, '0.1.0', { requires: ['base', 'oop']})" | ./funk YUI.add 2;
	@echo "YUI.add('test', function (Y, NAME) { console.log(NAME); }, '0.1.0', { requires: ['base', 'oop']})" | ./funk YUI.add 3;
	@echo "YUI.add('test', function (Y, NAME) { console.log(NAME); }, '0.1.0', { requires: ['base', 'oop']})" | ./funk YUI.add 4;
	@echo "YUI.add('test', function (Y, NAME) { console.log(NAME); }, '0.1.0', { requires: ['base', 'oop']})" | ./funk YUI.add 1 YUI.add 2 YUI.add 3 YUI.add 4;

clean:
	@rm -fv ./funk
