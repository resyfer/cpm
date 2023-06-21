// Requires Git to function

/*

cpm.build:
[global]
policy = 0.1
type = lib-static / lib-dynamic / bin
headers = /path1, /path2, ...
src = /path1, /path2, ...
out = outname // libraries are named as liboutname, and bin are named as outname

[dependencies]
<pkg>:<ver> = 0.0.5 // Fixed version
<pkg>:<ver> = ^0.0.5 // 0.0.x, x >= 5, patches only, default
<pkg>:<ver> = ~0.0.5 // 0.x.y, minor fixes only
<pkg>:<ver> = * // Latest
<pkg>:<ver> = <0.0.5, // version < 0.0.5

[versions]
0.0.1 = <commit-hash> or <branch>
0.0.2 = <commit-hash> or <branch>
1.0.0 = <commit-hash> or <branch>
1.0.1 = <commit-hash> or <branch>

*/

/*

cpm.lock:
[global]
policy = 0.1

[pkg1:ver]
url = <git repo URL>
location = <actual location of local clone of repo>
dependents = <count>

...

*/

#include <stdio.h>

#include <args.h>

int main(int argc, char** argv) {

	printf("Hello World\n");

	parse_cli_args(argc, argv);

	printf("Version: %d\n", arguments.version);
	printf("Help: %d\n", arguments.help);
	printf("Init: %d\n", arguments.init);

	printf("Add: ");
	for(int i = 0; i < arguments.n_add; i++) {
		printf("%s ", arguments.add[i]);
	}
	printf("\n");


	return 0;
}