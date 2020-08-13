・pthred.hで定義されている関数を使用する時には、"-pthread"をオプションとして追加
　ex) gcc test.c -pthread

・ビルドのためのgccコマンド(tcpの場合)
gcc -o MyEth main.c param.c sock.c ether.c arp.c ip.c icmp.c udp.c tcp.c dhcp.c cmd.c -pthread

・-o オプションについて
例えば、
 gcc -o test test.c
なら
 test.cをコンパイル、リンクして「tes」tという実行ファイルを作成

-o を指定しなければ、実行ファイルは、「a.out」で作成される