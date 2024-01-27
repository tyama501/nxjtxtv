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
  
misaki0.bin - misakia.bin,  
misaki0o.bin - misakiao.binをカレントディレクトリまたは/lib/font/に置いてください。  
(misaki*o.binはunicodeからフォント格納位置に変換するためのものです。)  

## How to use
./nxjtxtv textfile  
(/bin/にインストールした場合はnxjtxtv textfile)  

    Key  
    n : next page  
    q : quit  

## Build
このテキストViewerのBuildにはELKSのBuild環境が必要です。  
ELKSのBuild環境構築後、elks/elkscmd/nano-X/demos/にnxjtxtv.cを置き、  
elks/elkscmd/nano-X/Makefileに以下を加えて下さい。  

    NANOXDEMOS += bin/nxjtxtv  
    ...  
    bin/nxjtxtv: demos/nxjtxtv.o $(NANOXFILES) $(SERVFILES)
            $(LD) $(LDFLAGS) -maout-heap=0xffff $(NANOXFILES) demos/nxjtxtv.o $(SERVFILES) $(NANOLIBS) $(LIBS) -o bin/nxjtxtv $(MATHLIB) $(LDLIBS)

elks/elkscmd/nano-X/でmake clean, makeを行うと  
elks/elkscmd/nano-X/bin/にnxjtxtvができます。
