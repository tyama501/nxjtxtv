# ELKS向け簡易UTF-8 日本語テキストViewer with Nano-X
Simple UTF-8 Japanese text file viewer with Nano-X for ELKS  
  
<img src=image/nxjtxtv_elks.png width="320pix">
  
Embeddable Linux Kernel Subset (ELKS)は以下で開発されている8086等Intel IA16アーキテクチャ向けのLinux-LikeなOSです。  
https://github.com/ghaerr/elks  

## Font 
以下で配布されている美咲フォント(misaki_bdf_2021-05-05)を複数のバイナリファイルに分割して使用しています。  
素晴らしいフォントの作成と配布をありがとうございます。  
https://littlelimit.net/misaki.htm  

misaki_gothic.bdfをnano-Xのリポジトリで公開されているconvbdfを改変したものを用いて変換しています。  
convbdf original  
https://github.com/ghaerr/microwindows/blob/master/src/fonts/tools/convbdf.c  

conbdf modified  
https://github.com/tyama501/nxjtxtv/blob/main/font/convbdf_out8.c  
  
misaki0.bin - misakib.bin,  
misaki0o.bin - misakibo.binをカレントディレクトリまたは/lib/font/に置いてください。  
(misaki*o.binはunicodeからフォント格納位置に変換するためのものです。)  

## How to use
./nxjtxtv textfile  
(/bin/にインストールした場合はnxjtxtv textfile)  

    Key  
    n : next page  
    q : quit  

-dで縦横2倍モードになります。  
nxjtxtv -d textfile  

sjisutf8等をpipeで繋いで表示することもメモリが足りればできます。  
sjisutf8 sjisfile | nxjtxtv  
https://github.com/tyama501/sjis-to-utf8_elks  

現状、Window manager上のnxtermからsjisutf8のパイプを用いて起動した場合、  
Windowを動かすと次のページに進んでしまうようです。  
Window managerを用いる場合は一度変換結果をファイルに落として開いてください。  

## Build
このテキストViewerのBuildにはELKSのBuild環境が必要です。  
Nano-XのWindow managerに対応したalpha_0_5以降のbuildにはmicrowindowsリポジトリのBuild環境も必要です。  
microwindowsリポジトリ  
https://github.com/ghaerr/microwindows

**(1) alpha_0_4以前のBuild方法**  
ELKSのBuild環境構築後、elks/elkscmd/nano-X/demos/にnxjtxtv.cを置き、  
elks/elkscmd/nano-X/Makefileに以下を加えて下さい。  

    NANOXDEMOS += bin/nxjtxtv  
    ...  
    bin/nxjtxtv: demos/nxjtxtv.o $(NANOXFILES) $(SERVFILES)
            $(LD) $(LDFLAGS) -maout-heap=0xffff $(NANOXFILES) demos/nxjtxtv.o $(SERVFILES) $(NANOLIBS) $(LIBS) -o bin/nxjtxtv $(MATHLIB) $(LDLIBS)

elks/elkscmd/nano-X/でmake clean, makeを行うと  
elks/elkscmd/nano-X/bin/にnxjtxtvができます。  
  
この場合のNano-XのソースコードはELKSのリポジトリをご参照ください。  
https://github.com/ghaerr/elks/tree/master/elkscmd/nano-X
  
**(2) alpha_0_5以降のBuild方法**  
ELKSのBuild環境で. ./env.shを行ってください。  
microwindowsリポジトリのBuild環境構築後、microwindows/src/demos/nanox/にnxjtxtv.cを置き、  
microwindows/src/demos/nanox/Makefile.elksに以下を加えて下さい。  

    $(BIN)nxjtxtv    \
    ...
    $(BIN)nxjtxtv: nxjtxtv.o $(NXLIB)
        $(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

PC-98向けにBuildする場合はmicrowindows/src/Makefile.elksを以下のように変更してください。  

    CONFIG_ARCH_IBMPC=n
    SCREEN_CGA=n
    CONFIG_ARCH_PC98=y

microwindows/srcでmake -f Makefile.elks clean, make -f Makefile.elksを行うと  
Buildされ生成されたnxjtxtvがmicrowindows/src/binに  
Nano-Xサーバーであるnano-xやWindow managerをスタートさせるnxstart等とともにできます。

この場合のNano-Xのソースコードはmicrowindowsのリポジトリをご参照ください。

