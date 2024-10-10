<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<title>dwm.c - dwm - dynamic window manager
</title>
<link rel="icon" type="image/png" href="../favicon.png" />
<link rel="alternate" type="application/atom+xml" title="dwm Atom Feed" href="../atom.xml" />
<link rel="alternate" type="application/atom+xml" title="dwm Atom Feed (tags)" href="../tags.xml" />
<link rel="stylesheet" type="text/css" href="../style.css" />
</head>
<body>
<table><tr><td><a href="../../"><img src="../logo.png" alt="" width="32" height="32" /></a></td><td><h1>dwm</h1><span class="desc">dynamic window manager
</span></td></tr><tr class="url"><td></td><td>git clone <a href="git://git.suckless.org/dwm">git://git.suckless.org/dwm</a></td></tr><tr><td></td><td>
<a href="../log.html">Log</a> | <a href="../files.html">Files</a> | <a href="../refs.html">Refs</a> | <a href="../file/README.html">README</a> | <a href="../file/LICENSE.html">LICENSE</a></td></tr></table>
<hr/>
<div id="content">
<p> dwm.c (53238B)</p><hr/><pre id="blob">
<a href="#l1" class="line" id="l1">      1</a> /* See LICENSE file for copyright and license details.
<a href="#l2" class="line" id="l2">      2</a>  *
<a href="#l3" class="line" id="l3">      3</a>  * dynamic window manager is designed like any other X client as well. It is
<a href="#l4" class="line" id="l4">      4</a>  * driven through handling X events. In contrast to other X clients, a window
<a href="#l5" class="line" id="l5">      5</a>  * manager selects for SubstructureRedirectMask on the root window, to receive
<a href="#l6" class="line" id="l6">      6</a>  * events about window (dis-)appearance. Only one X connection at a time is
<a href="#l7" class="line" id="l7">      7</a>  * allowed to select for this event mask.
<a href="#l8" class="line" id="l8">      8</a>  *
<a href="#l9" class="line" id="l9">      9</a>  * The event handlers of dwm are organized in an array which is accessed
<a href="#l10" class="line" id="l10">     10</a>  * whenever a new event has been fetched. This allows event dispatching
<a href="#l11" class="line" id="l11">     11</a>  * in O(1) time.
<a href="#l12" class="line" id="l12">     12</a>  *
<a href="#l13" class="line" id="l13">     13</a>  * Each child of the root window is called a client, except windows which have
<a href="#l14" class="line" id="l14">     14</a>  * set the override_redirect flag. Clients are organized in a linked client
<a href="#l15" class="line" id="l15">     15</a>  * list on each monitor, the focus history is remembered through a stack list
<a href="#l16" class="line" id="l16">     16</a>  * on each monitor. Each client contains a bit array to indicate the tags of a
<a href="#l17" class="line" id="l17">     17</a>  * client.
<a href="#l18" class="line" id="l18">     18</a>  *
<a href="#l19" class="line" id="l19">     19</a>  * Keys and tagging rules are organized as arrays and defined in config.h.
<a href="#l20" class="line" id="l20">     20</a>  *
<a href="#l21" class="line" id="l21">     21</a>  * To understand everything else, start reading main().
<a href="#l22" class="line" id="l22">     22</a>  */
<a href="#l23" class="line" id="l23">     23</a> #include &lt;errno.h&gt;
<a href="#l24" class="line" id="l24">     24</a> #include &lt;locale.h&gt;
<a href="#l25" class="line" id="l25">     25</a> #include &lt;signal.h&gt;
<a href="#l26" class="line" id="l26">     26</a> #include &lt;stdarg.h&gt;
<a href="#l27" class="line" id="l27">     27</a> #include &lt;stdio.h&gt;
<a href="#l28" class="line" id="l28">     28</a> #include &lt;stdlib.h&gt;
<a href="#l29" class="line" id="l29">     29</a> #include &lt;string.h&gt;
<a href="#l30" class="line" id="l30">     30</a> #include &lt;unistd.h&gt;
<a href="#l31" class="line" id="l31">     31</a> #include &lt;sys/types.h&gt;
<a href="#l32" class="line" id="l32">     32</a> #include &lt;sys/wait.h&gt;
<a href="#l33" class="line" id="l33">     33</a> #include &lt;X11/cursorfont.h&gt;
<a href="#l34" class="line" id="l34">     34</a> #include &lt;X11/keysym.h&gt;
<a href="#l35" class="line" id="l35">     35</a> #include &lt;X11/Xatom.h&gt;
<a href="#l36" class="line" id="l36">     36</a> #include &lt;X11/Xlib.h&gt;
<a href="#l37" class="line" id="l37">     37</a> #include &lt;X11/Xproto.h&gt;
<a href="#l38" class="line" id="l38">     38</a> #include &lt;X11/Xutil.h&gt;
<a href="#l39" class="line" id="l39">     39</a> #ifdef XINERAMA
<a href="#l40" class="line" id="l40">     40</a> #include &lt;X11/extensions/Xinerama.h&gt;
<a href="#l41" class="line" id="l41">     41</a> #endif /* XINERAMA */
<a href="#l42" class="line" id="l42">     42</a> #include &lt;X11/Xft/Xft.h&gt;
<a href="#l43" class="line" id="l43">     43</a> 
<a href="#l44" class="line" id="l44">     44</a> #include &quot;drw.h&quot;
<a href="#l45" class="line" id="l45">     45</a> #include &quot;util.h&quot;
<a href="#l46" class="line" id="l46">     46</a> 
<a href="#l47" class="line" id="l47">     47</a> /* macros */
<a href="#l48" class="line" id="l48">     48</a> #define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
<a href="#l49" class="line" id="l49">     49</a> #define CLEANMASK(mask)         (mask &amp; ~(numlockmask|LockMask) &amp; (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
<a href="#l50" class="line" id="l50">     50</a> #define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)-&gt;wx+(m)-&gt;ww) - MAX((x),(m)-&gt;wx)) \
<a href="#l51" class="line" id="l51">     51</a>                                * MAX(0, MIN((y)+(h),(m)-&gt;wy+(m)-&gt;wh) - MAX((y),(m)-&gt;wy)))
<a href="#l52" class="line" id="l52">     52</a> #define ISVISIBLE(C)            ((C-&gt;tags &amp; C-&gt;mon-&gt;tagset[C-&gt;mon-&gt;seltags]))
<a href="#l53" class="line" id="l53">     53</a> #define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
<a href="#l54" class="line" id="l54">     54</a> #define WIDTH(X)                ((X)-&gt;w + 2 * (X)-&gt;bw)
<a href="#l55" class="line" id="l55">     55</a> #define HEIGHT(X)               ((X)-&gt;h + 2 * (X)-&gt;bw)
<a href="#l56" class="line" id="l56">     56</a> #define TAGMASK                 ((1 &lt;&lt; LENGTH(tags)) - 1)
<a href="#l57" class="line" id="l57">     57</a> #define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)
<a href="#l58" class="line" id="l58">     58</a> 
<a href="#l59" class="line" id="l59">     59</a> /* enums */
<a href="#l60" class="line" id="l60">     60</a> enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
<a href="#l61" class="line" id="l61">     61</a> enum { SchemeNorm, SchemeSel }; /* color schemes */
<a href="#l62" class="line" id="l62">     62</a> enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
<a href="#l63" class="line" id="l63">     63</a>        NetWMFullscreen, NetActiveWindow, NetWMWindowType,
<a href="#l64" class="line" id="l64">     64</a>        NetWMWindowTypeDialog, NetClientList, NetLast }; /* EWMH atoms */
<a href="#l65" class="line" id="l65">     65</a> enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
<a href="#l66" class="line" id="l66">     66</a> enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
<a href="#l67" class="line" id="l67">     67</a>        ClkClientWin, ClkRootWin, ClkLast }; /* clicks */
<a href="#l68" class="line" id="l68">     68</a> 
<a href="#l69" class="line" id="l69">     69</a> typedef union {
<a href="#l70" class="line" id="l70">     70</a> 	int i;
<a href="#l71" class="line" id="l71">     71</a> 	unsigned int ui;
<a href="#l72" class="line" id="l72">     72</a> 	float f;
<a href="#l73" class="line" id="l73">     73</a> 	const void *v;
<a href="#l74" class="line" id="l74">     74</a> } Arg;
<a href="#l75" class="line" id="l75">     75</a> 
<a href="#l76" class="line" id="l76">     76</a> typedef struct {
<a href="#l77" class="line" id="l77">     77</a> 	unsigned int click;
<a href="#l78" class="line" id="l78">     78</a> 	unsigned int mask;
<a href="#l79" class="line" id="l79">     79</a> 	unsigned int button;
<a href="#l80" class="line" id="l80">     80</a> 	void (*func)(const Arg *arg);
<a href="#l81" class="line" id="l81">     81</a> 	const Arg arg;
<a href="#l82" class="line" id="l82">     82</a> } Button;
<a href="#l83" class="line" id="l83">     83</a> 
<a href="#l84" class="line" id="l84">     84</a> typedef struct Monitor Monitor;
<a href="#l85" class="line" id="l85">     85</a> typedef struct Client Client;
<a href="#l86" class="line" id="l86">     86</a> struct Client {
<a href="#l87" class="line" id="l87">     87</a> 	char name[256];
<a href="#l88" class="line" id="l88">     88</a> 	float mina, maxa;
<a href="#l89" class="line" id="l89">     89</a> 	int x, y, w, h;
<a href="#l90" class="line" id="l90">     90</a> 	int oldx, oldy, oldw, oldh;
<a href="#l91" class="line" id="l91">     91</a> 	int basew, baseh, incw, inch, maxw, maxh, minw, minh, hintsvalid;
<a href="#l92" class="line" id="l92">     92</a> 	int bw, oldbw;
<a href="#l93" class="line" id="l93">     93</a> 	unsigned int tags;
<a href="#l94" class="line" id="l94">     94</a> 	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
<a href="#l95" class="line" id="l95">     95</a> 	Client *next;
<a href="#l96" class="line" id="l96">     96</a> 	Client *snext;
<a href="#l97" class="line" id="l97">     97</a> 	Monitor *mon;
<a href="#l98" class="line" id="l98">     98</a> 	Window win;
<a href="#l99" class="line" id="l99">     99</a> };
<a href="#l100" class="line" id="l100">    100</a> 
<a href="#l101" class="line" id="l101">    101</a> typedef struct {
<a href="#l102" class="line" id="l102">    102</a> 	unsigned int mod;
<a href="#l103" class="line" id="l103">    103</a> 	KeySym keysym;
<a href="#l104" class="line" id="l104">    104</a> 	void (*func)(const Arg *);
<a href="#l105" class="line" id="l105">    105</a> 	const Arg arg;
<a href="#l106" class="line" id="l106">    106</a> } Key;
<a href="#l107" class="line" id="l107">    107</a> 
<a href="#l108" class="line" id="l108">    108</a> typedef struct {
<a href="#l109" class="line" id="l109">    109</a> 	const char *symbol;
<a href="#l110" class="line" id="l110">    110</a> 	void (*arrange)(Monitor *);
<a href="#l111" class="line" id="l111">    111</a> } Layout;
<a href="#l112" class="line" id="l112">    112</a> 
<a href="#l113" class="line" id="l113">    113</a> struct Monitor {
<a href="#l114" class="line" id="l114">    114</a> 	char ltsymbol[16];
<a href="#l115" class="line" id="l115">    115</a> 	float mfact;
<a href="#l116" class="line" id="l116">    116</a> 	int nmaster;
<a href="#l117" class="line" id="l117">    117</a> 	int num;
<a href="#l118" class="line" id="l118">    118</a> 	int by;               /* bar geometry */
<a href="#l119" class="line" id="l119">    119</a> 	int mx, my, mw, mh;   /* screen size */
<a href="#l120" class="line" id="l120">    120</a> 	int wx, wy, ww, wh;   /* window area  */
<a href="#l121" class="line" id="l121">    121</a> 	unsigned int seltags;
<a href="#l122" class="line" id="l122">    122</a> 	unsigned int sellt;
<a href="#l123" class="line" id="l123">    123</a> 	unsigned int tagset[2];
<a href="#l124" class="line" id="l124">    124</a> 	int showbar;
<a href="#l125" class="line" id="l125">    125</a> 	int topbar;
<a href="#l126" class="line" id="l126">    126</a> 	Client *clients;
<a href="#l127" class="line" id="l127">    127</a> 	Client *sel;
<a href="#l128" class="line" id="l128">    128</a> 	Client *stack;
<a href="#l129" class="line" id="l129">    129</a> 	Monitor *next;
<a href="#l130" class="line" id="l130">    130</a> 	Window barwin;
<a href="#l131" class="line" id="l131">    131</a> 	const Layout *lt[2];
<a href="#l132" class="line" id="l132">    132</a> };
<a href="#l133" class="line" id="l133">    133</a> 
<a href="#l134" class="line" id="l134">    134</a> typedef struct {
<a href="#l135" class="line" id="l135">    135</a> 	const char *class;
<a href="#l136" class="line" id="l136">    136</a> 	const char *instance;
<a href="#l137" class="line" id="l137">    137</a> 	const char *title;
<a href="#l138" class="line" id="l138">    138</a> 	unsigned int tags;
<a href="#l139" class="line" id="l139">    139</a> 	int isfloating;
<a href="#l140" class="line" id="l140">    140</a> 	int monitor;
<a href="#l141" class="line" id="l141">    141</a> } Rule;
<a href="#l142" class="line" id="l142">    142</a> 
<a href="#l143" class="line" id="l143">    143</a> /* function declarations */
<a href="#l144" class="line" id="l144">    144</a> static void applyrules(Client *c);
<a href="#l145" class="line" id="l145">    145</a> static int applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact);
<a href="#l146" class="line" id="l146">    146</a> static void arrange(Monitor *m);
<a href="#l147" class="line" id="l147">    147</a> static void arrangemon(Monitor *m);
<a href="#l148" class="line" id="l148">    148</a> static void attach(Client *c);
<a href="#l149" class="line" id="l149">    149</a> static void attachstack(Client *c);
<a href="#l150" class="line" id="l150">    150</a> static void buttonpress(XEvent *e);
<a href="#l151" class="line" id="l151">    151</a> static void checkotherwm(void);
<a href="#l152" class="line" id="l152">    152</a> static void cleanup(void);
<a href="#l153" class="line" id="l153">    153</a> static void cleanupmon(Monitor *mon);
<a href="#l154" class="line" id="l154">    154</a> static void clientmessage(XEvent *e);
<a href="#l155" class="line" id="l155">    155</a> static void configure(Client *c);
<a href="#l156" class="line" id="l156">    156</a> static void configurenotify(XEvent *e);
<a href="#l157" class="line" id="l157">    157</a> static void configurerequest(XEvent *e);
<a href="#l158" class="line" id="l158">    158</a> static Monitor *createmon(void);
<a href="#l159" class="line" id="l159">    159</a> static void destroynotify(XEvent *e);
<a href="#l160" class="line" id="l160">    160</a> static void detach(Client *c);
<a href="#l161" class="line" id="l161">    161</a> static void detachstack(Client *c);
<a href="#l162" class="line" id="l162">    162</a> static Monitor *dirtomon(int dir);
<a href="#l163" class="line" id="l163">    163</a> static void drawbar(Monitor *m);
<a href="#l164" class="line" id="l164">    164</a> static void drawbars(void);
<a href="#l165" class="line" id="l165">    165</a> static void enternotify(XEvent *e);
<a href="#l166" class="line" id="l166">    166</a> static void expose(XEvent *e);
<a href="#l167" class="line" id="l167">    167</a> static void focus(Client *c);
<a href="#l168" class="line" id="l168">    168</a> static void focusin(XEvent *e);
<a href="#l169" class="line" id="l169">    169</a> static void focusmon(const Arg *arg);
<a href="#l170" class="line" id="l170">    170</a> static void focusstack(const Arg *arg);
<a href="#l171" class="line" id="l171">    171</a> static Atom getatomprop(Client *c, Atom prop);
<a href="#l172" class="line" id="l172">    172</a> static int getrootptr(int *x, int *y);
<a href="#l173" class="line" id="l173">    173</a> static long getstate(Window w);
<a href="#l174" class="line" id="l174">    174</a> static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
<a href="#l175" class="line" id="l175">    175</a> static void grabbuttons(Client *c, int focused);
<a href="#l176" class="line" id="l176">    176</a> static void grabkeys(void);
<a href="#l177" class="line" id="l177">    177</a> static void incnmaster(const Arg *arg);
<a href="#l178" class="line" id="l178">    178</a> static void keypress(XEvent *e);
<a href="#l179" class="line" id="l179">    179</a> static void killclient(const Arg *arg);
<a href="#l180" class="line" id="l180">    180</a> static void manage(Window w, XWindowAttributes *wa);
<a href="#l181" class="line" id="l181">    181</a> static void mappingnotify(XEvent *e);
<a href="#l182" class="line" id="l182">    182</a> static void maprequest(XEvent *e);
<a href="#l183" class="line" id="l183">    183</a> static void monocle(Monitor *m);
<a href="#l184" class="line" id="l184">    184</a> static void motionnotify(XEvent *e);
<a href="#l185" class="line" id="l185">    185</a> static void movemouse(const Arg *arg);
<a href="#l186" class="line" id="l186">    186</a> static Client *nexttiled(Client *c);
<a href="#l187" class="line" id="l187">    187</a> static void pop(Client *c);
<a href="#l188" class="line" id="l188">    188</a> static void propertynotify(XEvent *e);
<a href="#l189" class="line" id="l189">    189</a> static void quit(const Arg *arg);
<a href="#l190" class="line" id="l190">    190</a> static Monitor *recttomon(int x, int y, int w, int h);
<a href="#l191" class="line" id="l191">    191</a> static void resize(Client *c, int x, int y, int w, int h, int interact);
<a href="#l192" class="line" id="l192">    192</a> static void resizeclient(Client *c, int x, int y, int w, int h);
<a href="#l193" class="line" id="l193">    193</a> static void resizemouse(const Arg *arg);
<a href="#l194" class="line" id="l194">    194</a> static void restack(Monitor *m);
<a href="#l195" class="line" id="l195">    195</a> static void run(void);
<a href="#l196" class="line" id="l196">    196</a> static void scan(void);
<a href="#l197" class="line" id="l197">    197</a> static int sendevent(Client *c, Atom proto);
<a href="#l198" class="line" id="l198">    198</a> static void sendmon(Client *c, Monitor *m);
<a href="#l199" class="line" id="l199">    199</a> static void setclientstate(Client *c, long state);
<a href="#l200" class="line" id="l200">    200</a> static void setfocus(Client *c);
<a href="#l201" class="line" id="l201">    201</a> static void setfullscreen(Client *c, int fullscreen);
<a href="#l202" class="line" id="l202">    202</a> static void setlayout(const Arg *arg);
<a href="#l203" class="line" id="l203">    203</a> static void setmfact(const Arg *arg);
<a href="#l204" class="line" id="l204">    204</a> static void setup(void);
<a href="#l205" class="line" id="l205">    205</a> static void seturgent(Client *c, int urg);
<a href="#l206" class="line" id="l206">    206</a> static void showhide(Client *c);
<a href="#l207" class="line" id="l207">    207</a> static void spawn(const Arg *arg);
<a href="#l208" class="line" id="l208">    208</a> static void tag(const Arg *arg);
<a href="#l209" class="line" id="l209">    209</a> static void tagmon(const Arg *arg);
<a href="#l210" class="line" id="l210">    210</a> static void tile(Monitor *m);
<a href="#l211" class="line" id="l211">    211</a> static void togglebar(const Arg *arg);
<a href="#l212" class="line" id="l212">    212</a> static void togglefloating(const Arg *arg);
<a href="#l213" class="line" id="l213">    213</a> static void toggletag(const Arg *arg);
<a href="#l214" class="line" id="l214">    214</a> static void toggleview(const Arg *arg);
<a href="#l215" class="line" id="l215">    215</a> static void unfocus(Client *c, int setfocus);
<a href="#l216" class="line" id="l216">    216</a> static void unmanage(Client *c, int destroyed);
<a href="#l217" class="line" id="l217">    217</a> static void unmapnotify(XEvent *e);
<a href="#l218" class="line" id="l218">    218</a> static void updatebarpos(Monitor *m);
<a href="#l219" class="line" id="l219">    219</a> static void updatebars(void);
<a href="#l220" class="line" id="l220">    220</a> static void updateclientlist(void);
<a href="#l221" class="line" id="l221">    221</a> static int updategeom(void);
<a href="#l222" class="line" id="l222">    222</a> static void updatenumlockmask(void);
<a href="#l223" class="line" id="l223">    223</a> static void updatesizehints(Client *c);
<a href="#l224" class="line" id="l224">    224</a> static void updatestatus(void);
<a href="#l225" class="line" id="l225">    225</a> static void updatetitle(Client *c);
<a href="#l226" class="line" id="l226">    226</a> static void updatewindowtype(Client *c);
<a href="#l227" class="line" id="l227">    227</a> static void updatewmhints(Client *c);
<a href="#l228" class="line" id="l228">    228</a> static void view(const Arg *arg);
<a href="#l229" class="line" id="l229">    229</a> static Client *wintoclient(Window w);
<a href="#l230" class="line" id="l230">    230</a> static Monitor *wintomon(Window w);
<a href="#l231" class="line" id="l231">    231</a> static int xerror(Display *dpy, XErrorEvent *ee);
<a href="#l232" class="line" id="l232">    232</a> static int xerrordummy(Display *dpy, XErrorEvent *ee);
<a href="#l233" class="line" id="l233">    233</a> static int xerrorstart(Display *dpy, XErrorEvent *ee);
<a href="#l234" class="line" id="l234">    234</a> static void zoom(const Arg *arg);
<a href="#l235" class="line" id="l235">    235</a> 
<a href="#l236" class="line" id="l236">    236</a> /* variables */
<a href="#l237" class="line" id="l237">    237</a> static const char broken[] = &quot;broken&quot;;
<a href="#l238" class="line" id="l238">    238</a> static char stext[256];
<a href="#l239" class="line" id="l239">    239</a> static int screen;
<a href="#l240" class="line" id="l240">    240</a> static int sw, sh;           /* X display screen geometry width, height */
<a href="#l241" class="line" id="l241">    241</a> static int bh;               /* bar height */
<a href="#l242" class="line" id="l242">    242</a> static int lrpad;            /* sum of left and right padding for text */
<a href="#l243" class="line" id="l243">    243</a> static int (*xerrorxlib)(Display *, XErrorEvent *);
<a href="#l244" class="line" id="l244">    244</a> static unsigned int numlockmask = 0;
<a href="#l245" class="line" id="l245">    245</a> static void (*handler[LASTEvent]) (XEvent *) = {
<a href="#l246" class="line" id="l246">    246</a> 	[ButtonPress] = buttonpress,
<a href="#l247" class="line" id="l247">    247</a> 	[ClientMessage] = clientmessage,
<a href="#l248" class="line" id="l248">    248</a> 	[ConfigureRequest] = configurerequest,
<a href="#l249" class="line" id="l249">    249</a> 	[ConfigureNotify] = configurenotify,
<a href="#l250" class="line" id="l250">    250</a> 	[DestroyNotify] = destroynotify,
<a href="#l251" class="line" id="l251">    251</a> 	[EnterNotify] = enternotify,
<a href="#l252" class="line" id="l252">    252</a> 	[Expose] = expose,
<a href="#l253" class="line" id="l253">    253</a> 	[FocusIn] = focusin,
<a href="#l254" class="line" id="l254">    254</a> 	[KeyPress] = keypress,
<a href="#l255" class="line" id="l255">    255</a> 	[MappingNotify] = mappingnotify,
<a href="#l256" class="line" id="l256">    256</a> 	[MapRequest] = maprequest,
<a href="#l257" class="line" id="l257">    257</a> 	[MotionNotify] = motionnotify,
<a href="#l258" class="line" id="l258">    258</a> 	[PropertyNotify] = propertynotify,
<a href="#l259" class="line" id="l259">    259</a> 	[UnmapNotify] = unmapnotify
<a href="#l260" class="line" id="l260">    260</a> };
<a href="#l261" class="line" id="l261">    261</a> static Atom wmatom[WMLast], netatom[NetLast];
<a href="#l262" class="line" id="l262">    262</a> static int running = 1;
<a href="#l263" class="line" id="l263">    263</a> static Cur *cursor[CurLast];
<a href="#l264" class="line" id="l264">    264</a> static Clr **scheme;
<a href="#l265" class="line" id="l265">    265</a> static Display *dpy;
<a href="#l266" class="line" id="l266">    266</a> static Drw *drw;
<a href="#l267" class="line" id="l267">    267</a> static Monitor *mons, *selmon;
<a href="#l268" class="line" id="l268">    268</a> static Window root, wmcheckwin;
<a href="#l269" class="line" id="l269">    269</a> 
<a href="#l270" class="line" id="l270">    270</a> /* configuration, allows nested code to access above variables */
<a href="#l271" class="line" id="l271">    271</a> #include &quot;config.h&quot;
<a href="#l272" class="line" id="l272">    272</a> 
<a href="#l273" class="line" id="l273">    273</a> /* compile-time check if all tags fit into an unsigned int bit array. */
<a href="#l274" class="line" id="l274">    274</a> struct NumTags { char limitexceeded[LENGTH(tags) &gt; 31 ? -1 : 1]; };
<a href="#l275" class="line" id="l275">    275</a> 
<a href="#l276" class="line" id="l276">    276</a> /* function implementations */
<a href="#l277" class="line" id="l277">    277</a> void
<a href="#l278" class="line" id="l278">    278</a> applyrules(Client *c)
<a href="#l279" class="line" id="l279">    279</a> {
<a href="#l280" class="line" id="l280">    280</a> 	const char *class, *instance;
<a href="#l281" class="line" id="l281">    281</a> 	unsigned int i;
<a href="#l282" class="line" id="l282">    282</a> 	const Rule *r;
<a href="#l283" class="line" id="l283">    283</a> 	Monitor *m;
<a href="#l284" class="line" id="l284">    284</a> 	XClassHint ch = { NULL, NULL };
<a href="#l285" class="line" id="l285">    285</a> 
<a href="#l286" class="line" id="l286">    286</a> 	/* rule matching */
<a href="#l287" class="line" id="l287">    287</a> 	c-&gt;isfloating = 0;
<a href="#l288" class="line" id="l288">    288</a> 	c-&gt;tags = 0;
<a href="#l289" class="line" id="l289">    289</a> 	XGetClassHint(dpy, c-&gt;win, &amp;ch);
<a href="#l290" class="line" id="l290">    290</a> 	class    = ch.res_class ? ch.res_class : broken;
<a href="#l291" class="line" id="l291">    291</a> 	instance = ch.res_name  ? ch.res_name  : broken;
<a href="#l292" class="line" id="l292">    292</a> 
<a href="#l293" class="line" id="l293">    293</a> 	for (i = 0; i &lt; LENGTH(rules); i++) {
<a href="#l294" class="line" id="l294">    294</a> 		r = &amp;rules[i];
<a href="#l295" class="line" id="l295">    295</a> 		if ((!r-&gt;title || strstr(c-&gt;name, r-&gt;title))
<a href="#l296" class="line" id="l296">    296</a> 		&amp;&amp; (!r-&gt;class || strstr(class, r-&gt;class))
<a href="#l297" class="line" id="l297">    297</a> 		&amp;&amp; (!r-&gt;instance || strstr(instance, r-&gt;instance)))
<a href="#l298" class="line" id="l298">    298</a> 		{
<a href="#l299" class="line" id="l299">    299</a> 			c-&gt;isfloating = r-&gt;isfloating;
<a href="#l300" class="line" id="l300">    300</a> 			c-&gt;tags |= r-&gt;tags;
<a href="#l301" class="line" id="l301">    301</a> 			for (m = mons; m &amp;&amp; m-&gt;num != r-&gt;monitor; m = m-&gt;next);
<a href="#l302" class="line" id="l302">    302</a> 			if (m)
<a href="#l303" class="line" id="l303">    303</a> 				c-&gt;mon = m;
<a href="#l304" class="line" id="l304">    304</a> 		}
<a href="#l305" class="line" id="l305">    305</a> 	}
<a href="#l306" class="line" id="l306">    306</a> 	if (ch.res_class)
<a href="#l307" class="line" id="l307">    307</a> 		XFree(ch.res_class);
<a href="#l308" class="line" id="l308">    308</a> 	if (ch.res_name)
<a href="#l309" class="line" id="l309">    309</a> 		XFree(ch.res_name);
<a href="#l310" class="line" id="l310">    310</a> 	c-&gt;tags = c-&gt;tags &amp; TAGMASK ? c-&gt;tags &amp; TAGMASK : c-&gt;mon-&gt;tagset[c-&gt;mon-&gt;seltags];
<a href="#l311" class="line" id="l311">    311</a> }
<a href="#l312" class="line" id="l312">    312</a> 
<a href="#l313" class="line" id="l313">    313</a> int
<a href="#l314" class="line" id="l314">    314</a> applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact)
<a href="#l315" class="line" id="l315">    315</a> {
<a href="#l316" class="line" id="l316">    316</a> 	int baseismin;
<a href="#l317" class="line" id="l317">    317</a> 	Monitor *m = c-&gt;mon;
<a href="#l318" class="line" id="l318">    318</a> 
<a href="#l319" class="line" id="l319">    319</a> 	/* set minimum possible */
<a href="#l320" class="line" id="l320">    320</a> 	*w = MAX(1, *w);
<a href="#l321" class="line" id="l321">    321</a> 	*h = MAX(1, *h);
<a href="#l322" class="line" id="l322">    322</a> 	if (interact) {
<a href="#l323" class="line" id="l323">    323</a> 		if (*x &gt; sw)
<a href="#l324" class="line" id="l324">    324</a> 			*x = sw - WIDTH(c);
<a href="#l325" class="line" id="l325">    325</a> 		if (*y &gt; sh)
<a href="#l326" class="line" id="l326">    326</a> 			*y = sh - HEIGHT(c);
<a href="#l327" class="line" id="l327">    327</a> 		if (*x + *w + 2 * c-&gt;bw &lt; 0)
<a href="#l328" class="line" id="l328">    328</a> 			*x = 0;
<a href="#l329" class="line" id="l329">    329</a> 		if (*y + *h + 2 * c-&gt;bw &lt; 0)
<a href="#l330" class="line" id="l330">    330</a> 			*y = 0;
<a href="#l331" class="line" id="l331">    331</a> 	} else {
<a href="#l332" class="line" id="l332">    332</a> 		if (*x &gt;= m-&gt;wx + m-&gt;ww)
<a href="#l333" class="line" id="l333">    333</a> 			*x = m-&gt;wx + m-&gt;ww - WIDTH(c);
<a href="#l334" class="line" id="l334">    334</a> 		if (*y &gt;= m-&gt;wy + m-&gt;wh)
<a href="#l335" class="line" id="l335">    335</a> 			*y = m-&gt;wy + m-&gt;wh - HEIGHT(c);
<a href="#l336" class="line" id="l336">    336</a> 		if (*x + *w + 2 * c-&gt;bw &lt;= m-&gt;wx)
<a href="#l337" class="line" id="l337">    337</a> 			*x = m-&gt;wx;
<a href="#l338" class="line" id="l338">    338</a> 		if (*y + *h + 2 * c-&gt;bw &lt;= m-&gt;wy)
<a href="#l339" class="line" id="l339">    339</a> 			*y = m-&gt;wy;
<a href="#l340" class="line" id="l340">    340</a> 	}
<a href="#l341" class="line" id="l341">    341</a> 	if (*h &lt; bh)
<a href="#l342" class="line" id="l342">    342</a> 		*h = bh;
<a href="#l343" class="line" id="l343">    343</a> 	if (*w &lt; bh)
<a href="#l344" class="line" id="l344">    344</a> 		*w = bh;
<a href="#l345" class="line" id="l345">    345</a> 	if (resizehints || c-&gt;isfloating || !c-&gt;mon-&gt;lt[c-&gt;mon-&gt;sellt]-&gt;arrange) {
<a href="#l346" class="line" id="l346">    346</a> 		if (!c-&gt;hintsvalid)
<a href="#l347" class="line" id="l347">    347</a> 			updatesizehints(c);
<a href="#l348" class="line" id="l348">    348</a> 		/* see last two sentences in ICCCM 4.1.2.3 */
<a href="#l349" class="line" id="l349">    349</a> 		baseismin = c-&gt;basew == c-&gt;minw &amp;&amp; c-&gt;baseh == c-&gt;minh;
<a href="#l350" class="line" id="l350">    350</a> 		if (!baseismin) { /* temporarily remove base dimensions */
<a href="#l351" class="line" id="l351">    351</a> 			*w -= c-&gt;basew;
<a href="#l352" class="line" id="l352">    352</a> 			*h -= c-&gt;baseh;
<a href="#l353" class="line" id="l353">    353</a> 		}
<a href="#l354" class="line" id="l354">    354</a> 		/* adjust for aspect limits */
<a href="#l355" class="line" id="l355">    355</a> 		if (c-&gt;mina &gt; 0 &amp;&amp; c-&gt;maxa &gt; 0) {
<a href="#l356" class="line" id="l356">    356</a> 			if (c-&gt;maxa &lt; (float)*w / *h)
<a href="#l357" class="line" id="l357">    357</a> 				*w = *h * c-&gt;maxa + 0.5;
<a href="#l358" class="line" id="l358">    358</a> 			else if (c-&gt;mina &lt; (float)*h / *w)
<a href="#l359" class="line" id="l359">    359</a> 				*h = *w * c-&gt;mina + 0.5;
<a href="#l360" class="line" id="l360">    360</a> 		}
<a href="#l361" class="line" id="l361">    361</a> 		if (baseismin) { /* increment calculation requires this */
<a href="#l362" class="line" id="l362">    362</a> 			*w -= c-&gt;basew;
<a href="#l363" class="line" id="l363">    363</a> 			*h -= c-&gt;baseh;
<a href="#l364" class="line" id="l364">    364</a> 		}
<a href="#l365" class="line" id="l365">    365</a> 		/* adjust for increment value */
<a href="#l366" class="line" id="l366">    366</a> 		if (c-&gt;incw)
<a href="#l367" class="line" id="l367">    367</a> 			*w -= *w % c-&gt;incw;
<a href="#l368" class="line" id="l368">    368</a> 		if (c-&gt;inch)
<a href="#l369" class="line" id="l369">    369</a> 			*h -= *h % c-&gt;inch;
<a href="#l370" class="line" id="l370">    370</a> 		/* restore base dimensions */
<a href="#l371" class="line" id="l371">    371</a> 		*w = MAX(*w + c-&gt;basew, c-&gt;minw);
<a href="#l372" class="line" id="l372">    372</a> 		*h = MAX(*h + c-&gt;baseh, c-&gt;minh);
<a href="#l373" class="line" id="l373">    373</a> 		if (c-&gt;maxw)
<a href="#l374" class="line" id="l374">    374</a> 			*w = MIN(*w, c-&gt;maxw);
<a href="#l375" class="line" id="l375">    375</a> 		if (c-&gt;maxh)
<a href="#l376" class="line" id="l376">    376</a> 			*h = MIN(*h, c-&gt;maxh);
<a href="#l377" class="line" id="l377">    377</a> 	}
<a href="#l378" class="line" id="l378">    378</a> 	return *x != c-&gt;x || *y != c-&gt;y || *w != c-&gt;w || *h != c-&gt;h;
<a href="#l379" class="line" id="l379">    379</a> }
<a href="#l380" class="line" id="l380">    380</a> 
<a href="#l381" class="line" id="l381">    381</a> void
<a href="#l382" class="line" id="l382">    382</a> arrange(Monitor *m)
<a href="#l383" class="line" id="l383">    383</a> {
<a href="#l384" class="line" id="l384">    384</a> 	if (m)
<a href="#l385" class="line" id="l385">    385</a> 		showhide(m-&gt;stack);
<a href="#l386" class="line" id="l386">    386</a> 	else for (m = mons; m; m = m-&gt;next)
<a href="#l387" class="line" id="l387">    387</a> 		showhide(m-&gt;stack);
<a href="#l388" class="line" id="l388">    388</a> 	if (m) {
<a href="#l389" class="line" id="l389">    389</a> 		arrangemon(m);
<a href="#l390" class="line" id="l390">    390</a> 		restack(m);
<a href="#l391" class="line" id="l391">    391</a> 	} else for (m = mons; m; m = m-&gt;next)
<a href="#l392" class="line" id="l392">    392</a> 		arrangemon(m);
<a href="#l393" class="line" id="l393">    393</a> }
<a href="#l394" class="line" id="l394">    394</a> 
<a href="#l395" class="line" id="l395">    395</a> void
<a href="#l396" class="line" id="l396">    396</a> arrangemon(Monitor *m)
<a href="#l397" class="line" id="l397">    397</a> {
<a href="#l398" class="line" id="l398">    398</a> 	strncpy(m-&gt;ltsymbol, m-&gt;lt[m-&gt;sellt]-&gt;symbol, sizeof m-&gt;ltsymbol);
<a href="#l399" class="line" id="l399">    399</a> 	if (m-&gt;lt[m-&gt;sellt]-&gt;arrange)
<a href="#l400" class="line" id="l400">    400</a> 		m-&gt;lt[m-&gt;sellt]-&gt;arrange(m);
<a href="#l401" class="line" id="l401">    401</a> }
<a href="#l402" class="line" id="l402">    402</a> 
<a href="#l403" class="line" id="l403">    403</a> void
<a href="#l404" class="line" id="l404">    404</a> attach(Client *c)
<a href="#l405" class="line" id="l405">    405</a> {
<a href="#l406" class="line" id="l406">    406</a> 	c-&gt;next = c-&gt;mon-&gt;clients;
<a href="#l407" class="line" id="l407">    407</a> 	c-&gt;mon-&gt;clients = c;
<a href="#l408" class="line" id="l408">    408</a> }
<a href="#l409" class="line" id="l409">    409</a> 
<a href="#l410" class="line" id="l410">    410</a> void
<a href="#l411" class="line" id="l411">    411</a> attachstack(Client *c)
<a href="#l412" class="line" id="l412">    412</a> {
<a href="#l413" class="line" id="l413">    413</a> 	c-&gt;snext = c-&gt;mon-&gt;stack;
<a href="#l414" class="line" id="l414">    414</a> 	c-&gt;mon-&gt;stack = c;
<a href="#l415" class="line" id="l415">    415</a> }
<a href="#l416" class="line" id="l416">    416</a> 
<a href="#l417" class="line" id="l417">    417</a> void
<a href="#l418" class="line" id="l418">    418</a> buttonpress(XEvent *e)
<a href="#l419" class="line" id="l419">    419</a> {
<a href="#l420" class="line" id="l420">    420</a> 	unsigned int i, x, click;
<a href="#l421" class="line" id="l421">    421</a> 	Arg arg = {0};
<a href="#l422" class="line" id="l422">    422</a> 	Client *c;
<a href="#l423" class="line" id="l423">    423</a> 	Monitor *m;
<a href="#l424" class="line" id="l424">    424</a> 	XButtonPressedEvent *ev = &amp;e-&gt;xbutton;
<a href="#l425" class="line" id="l425">    425</a> 
<a href="#l426" class="line" id="l426">    426</a> 	click = ClkRootWin;
<a href="#l427" class="line" id="l427">    427</a> 	/* focus monitor if necessary */
<a href="#l428" class="line" id="l428">    428</a> 	if ((m = wintomon(ev-&gt;window)) &amp;&amp; m != selmon) {
<a href="#l429" class="line" id="l429">    429</a> 		unfocus(selmon-&gt;sel, 1);
<a href="#l430" class="line" id="l430">    430</a> 		selmon = m;
<a href="#l431" class="line" id="l431">    431</a> 		focus(NULL);
<a href="#l432" class="line" id="l432">    432</a> 	}
<a href="#l433" class="line" id="l433">    433</a> 	if (ev-&gt;window == selmon-&gt;barwin) {
<a href="#l434" class="line" id="l434">    434</a> 		i = x = 0;
<a href="#l435" class="line" id="l435">    435</a> 		do
<a href="#l436" class="line" id="l436">    436</a> 			x += TEXTW(tags[i]);
<a href="#l437" class="line" id="l437">    437</a> 		while (ev-&gt;x &gt;= x &amp;&amp; ++i &lt; LENGTH(tags));
<a href="#l438" class="line" id="l438">    438</a> 		if (i &lt; LENGTH(tags)) {
<a href="#l439" class="line" id="l439">    439</a> 			click = ClkTagBar;
<a href="#l440" class="line" id="l440">    440</a> 			arg.ui = 1 &lt;&lt; i;
<a href="#l441" class="line" id="l441">    441</a> 		} else if (ev-&gt;x &lt; x + TEXTW(selmon-&gt;ltsymbol))
<a href="#l442" class="line" id="l442">    442</a> 			click = ClkLtSymbol;
<a href="#l443" class="line" id="l443">    443</a> 		else if (ev-&gt;x &gt; selmon-&gt;ww - (int)TEXTW(stext))
<a href="#l444" class="line" id="l444">    444</a> 			click = ClkStatusText;
<a href="#l445" class="line" id="l445">    445</a> 		else
<a href="#l446" class="line" id="l446">    446</a> 			click = ClkWinTitle;
<a href="#l447" class="line" id="l447">    447</a> 	} else if ((c = wintoclient(ev-&gt;window))) {
<a href="#l448" class="line" id="l448">    448</a> 		focus(c);
<a href="#l449" class="line" id="l449">    449</a> 		restack(selmon);
<a href="#l450" class="line" id="l450">    450</a> 		XAllowEvents(dpy, ReplayPointer, CurrentTime);
<a href="#l451" class="line" id="l451">    451</a> 		click = ClkClientWin;
<a href="#l452" class="line" id="l452">    452</a> 	}
<a href="#l453" class="line" id="l453">    453</a> 	for (i = 0; i &lt; LENGTH(buttons); i++)
<a href="#l454" class="line" id="l454">    454</a> 		if (click == buttons[i].click &amp;&amp; buttons[i].func &amp;&amp; buttons[i].button == ev-&gt;button
<a href="#l455" class="line" id="l455">    455</a> 		&amp;&amp; CLEANMASK(buttons[i].mask) == CLEANMASK(ev-&gt;state))
<a href="#l456" class="line" id="l456">    456</a> 			buttons[i].func(click == ClkTagBar &amp;&amp; buttons[i].arg.i == 0 ? &amp;arg : &amp;buttons[i].arg);
<a href="#l457" class="line" id="l457">    457</a> }
<a href="#l458" class="line" id="l458">    458</a> 
<a href="#l459" class="line" id="l459">    459</a> void
<a href="#l460" class="line" id="l460">    460</a> checkotherwm(void)
<a href="#l461" class="line" id="l461">    461</a> {
<a href="#l462" class="line" id="l462">    462</a> 	xerrorxlib = XSetErrorHandler(xerrorstart);
<a href="#l463" class="line" id="l463">    463</a> 	/* this causes an error if some other window manager is running */
<a href="#l464" class="line" id="l464">    464</a> 	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
<a href="#l465" class="line" id="l465">    465</a> 	XSync(dpy, False);
<a href="#l466" class="line" id="l466">    466</a> 	XSetErrorHandler(xerror);
<a href="#l467" class="line" id="l467">    467</a> 	XSync(dpy, False);
<a href="#l468" class="line" id="l468">    468</a> }
<a href="#l469" class="line" id="l469">    469</a> 
<a href="#l470" class="line" id="l470">    470</a> void
<a href="#l471" class="line" id="l471">    471</a> cleanup(void)
<a href="#l472" class="line" id="l472">    472</a> {
<a href="#l473" class="line" id="l473">    473</a> 	Arg a = {.ui = ~0};
<a href="#l474" class="line" id="l474">    474</a> 	Layout foo = { &quot;&quot;, NULL };
<a href="#l475" class="line" id="l475">    475</a> 	Monitor *m;
<a href="#l476" class="line" id="l476">    476</a> 	size_t i;
<a href="#l477" class="line" id="l477">    477</a> 
<a href="#l478" class="line" id="l478">    478</a> 	view(&amp;a);
<a href="#l479" class="line" id="l479">    479</a> 	selmon-&gt;lt[selmon-&gt;sellt] = &amp;foo;
<a href="#l480" class="line" id="l480">    480</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l481" class="line" id="l481">    481</a> 		while (m-&gt;stack)
<a href="#l482" class="line" id="l482">    482</a> 			unmanage(m-&gt;stack, 0);
<a href="#l483" class="line" id="l483">    483</a> 	XUngrabKey(dpy, AnyKey, AnyModifier, root);
<a href="#l484" class="line" id="l484">    484</a> 	while (mons)
<a href="#l485" class="line" id="l485">    485</a> 		cleanupmon(mons);
<a href="#l486" class="line" id="l486">    486</a> 	for (i = 0; i &lt; CurLast; i++)
<a href="#l487" class="line" id="l487">    487</a> 		drw_cur_free(drw, cursor[i]);
<a href="#l488" class="line" id="l488">    488</a> 	for (i = 0; i &lt; LENGTH(colors); i++)
<a href="#l489" class="line" id="l489">    489</a> 		free(scheme[i]);
<a href="#l490" class="line" id="l490">    490</a> 	free(scheme);
<a href="#l491" class="line" id="l491">    491</a> 	XDestroyWindow(dpy, wmcheckwin);
<a href="#l492" class="line" id="l492">    492</a> 	drw_free(drw);
<a href="#l493" class="line" id="l493">    493</a> 	XSync(dpy, False);
<a href="#l494" class="line" id="l494">    494</a> 	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
<a href="#l495" class="line" id="l495">    495</a> 	XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
<a href="#l496" class="line" id="l496">    496</a> }
<a href="#l497" class="line" id="l497">    497</a> 
<a href="#l498" class="line" id="l498">    498</a> void
<a href="#l499" class="line" id="l499">    499</a> cleanupmon(Monitor *mon)
<a href="#l500" class="line" id="l500">    500</a> {
<a href="#l501" class="line" id="l501">    501</a> 	Monitor *m;
<a href="#l502" class="line" id="l502">    502</a> 
<a href="#l503" class="line" id="l503">    503</a> 	if (mon == mons)
<a href="#l504" class="line" id="l504">    504</a> 		mons = mons-&gt;next;
<a href="#l505" class="line" id="l505">    505</a> 	else {
<a href="#l506" class="line" id="l506">    506</a> 		for (m = mons; m &amp;&amp; m-&gt;next != mon; m = m-&gt;next);
<a href="#l507" class="line" id="l507">    507</a> 		m-&gt;next = mon-&gt;next;
<a href="#l508" class="line" id="l508">    508</a> 	}
<a href="#l509" class="line" id="l509">    509</a> 	XUnmapWindow(dpy, mon-&gt;barwin);
<a href="#l510" class="line" id="l510">    510</a> 	XDestroyWindow(dpy, mon-&gt;barwin);
<a href="#l511" class="line" id="l511">    511</a> 	free(mon);
<a href="#l512" class="line" id="l512">    512</a> }
<a href="#l513" class="line" id="l513">    513</a> 
<a href="#l514" class="line" id="l514">    514</a> void
<a href="#l515" class="line" id="l515">    515</a> clientmessage(XEvent *e)
<a href="#l516" class="line" id="l516">    516</a> {
<a href="#l517" class="line" id="l517">    517</a> 	XClientMessageEvent *cme = &amp;e-&gt;xclient;
<a href="#l518" class="line" id="l518">    518</a> 	Client *c = wintoclient(cme-&gt;window);
<a href="#l519" class="line" id="l519">    519</a> 
<a href="#l520" class="line" id="l520">    520</a> 	if (!c)
<a href="#l521" class="line" id="l521">    521</a> 		return;
<a href="#l522" class="line" id="l522">    522</a> 	if (cme-&gt;message_type == netatom[NetWMState]) {
<a href="#l523" class="line" id="l523">    523</a> 		if (cme-&gt;data.l[1] == netatom[NetWMFullscreen]
<a href="#l524" class="line" id="l524">    524</a> 		|| cme-&gt;data.l[2] == netatom[NetWMFullscreen])
<a href="#l525" class="line" id="l525">    525</a> 			setfullscreen(c, (cme-&gt;data.l[0] == 1 /* _NET_WM_STATE_ADD    */
<a href="#l526" class="line" id="l526">    526</a> 				|| (cme-&gt;data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ &amp;&amp; !c-&gt;isfullscreen)));
<a href="#l527" class="line" id="l527">    527</a> 	} else if (cme-&gt;message_type == netatom[NetActiveWindow]) {
<a href="#l528" class="line" id="l528">    528</a> 		if (c != selmon-&gt;sel &amp;&amp; !c-&gt;isurgent)
<a href="#l529" class="line" id="l529">    529</a> 			seturgent(c, 1);
<a href="#l530" class="line" id="l530">    530</a> 	}
<a href="#l531" class="line" id="l531">    531</a> }
<a href="#l532" class="line" id="l532">    532</a> 
<a href="#l533" class="line" id="l533">    533</a> void
<a href="#l534" class="line" id="l534">    534</a> configure(Client *c)
<a href="#l535" class="line" id="l535">    535</a> {
<a href="#l536" class="line" id="l536">    536</a> 	XConfigureEvent ce;
<a href="#l537" class="line" id="l537">    537</a> 
<a href="#l538" class="line" id="l538">    538</a> 	ce.type = ConfigureNotify;
<a href="#l539" class="line" id="l539">    539</a> 	ce.display = dpy;
<a href="#l540" class="line" id="l540">    540</a> 	ce.event = c-&gt;win;
<a href="#l541" class="line" id="l541">    541</a> 	ce.window = c-&gt;win;
<a href="#l542" class="line" id="l542">    542</a> 	ce.x = c-&gt;x;
<a href="#l543" class="line" id="l543">    543</a> 	ce.y = c-&gt;y;
<a href="#l544" class="line" id="l544">    544</a> 	ce.width = c-&gt;w;
<a href="#l545" class="line" id="l545">    545</a> 	ce.height = c-&gt;h;
<a href="#l546" class="line" id="l546">    546</a> 	ce.border_width = c-&gt;bw;
<a href="#l547" class="line" id="l547">    547</a> 	ce.above = None;
<a href="#l548" class="line" id="l548">    548</a> 	ce.override_redirect = False;
<a href="#l549" class="line" id="l549">    549</a> 	XSendEvent(dpy, c-&gt;win, False, StructureNotifyMask, (XEvent *)&amp;ce);
<a href="#l550" class="line" id="l550">    550</a> }
<a href="#l551" class="line" id="l551">    551</a> 
<a href="#l552" class="line" id="l552">    552</a> void
<a href="#l553" class="line" id="l553">    553</a> configurenotify(XEvent *e)
<a href="#l554" class="line" id="l554">    554</a> {
<a href="#l555" class="line" id="l555">    555</a> 	Monitor *m;
<a href="#l556" class="line" id="l556">    556</a> 	Client *c;
<a href="#l557" class="line" id="l557">    557</a> 	XConfigureEvent *ev = &amp;e-&gt;xconfigure;
<a href="#l558" class="line" id="l558">    558</a> 	int dirty;
<a href="#l559" class="line" id="l559">    559</a> 
<a href="#l560" class="line" id="l560">    560</a> 	/* TODO: updategeom handling sucks, needs to be simplified */
<a href="#l561" class="line" id="l561">    561</a> 	if (ev-&gt;window == root) {
<a href="#l562" class="line" id="l562">    562</a> 		dirty = (sw != ev-&gt;width || sh != ev-&gt;height);
<a href="#l563" class="line" id="l563">    563</a> 		sw = ev-&gt;width;
<a href="#l564" class="line" id="l564">    564</a> 		sh = ev-&gt;height;
<a href="#l565" class="line" id="l565">    565</a> 		if (updategeom() || dirty) {
<a href="#l566" class="line" id="l566">    566</a> 			drw_resize(drw, sw, bh);
<a href="#l567" class="line" id="l567">    567</a> 			updatebars();
<a href="#l568" class="line" id="l568">    568</a> 			for (m = mons; m; m = m-&gt;next) {
<a href="#l569" class="line" id="l569">    569</a> 				for (c = m-&gt;clients; c; c = c-&gt;next)
<a href="#l570" class="line" id="l570">    570</a> 					if (c-&gt;isfullscreen)
<a href="#l571" class="line" id="l571">    571</a> 						resizeclient(c, m-&gt;mx, m-&gt;my, m-&gt;mw, m-&gt;mh);
<a href="#l572" class="line" id="l572">    572</a> 				XMoveResizeWindow(dpy, m-&gt;barwin, m-&gt;wx, m-&gt;by, m-&gt;ww, bh);
<a href="#l573" class="line" id="l573">    573</a> 			}
<a href="#l574" class="line" id="l574">    574</a> 			focus(NULL);
<a href="#l575" class="line" id="l575">    575</a> 			arrange(NULL);
<a href="#l576" class="line" id="l576">    576</a> 		}
<a href="#l577" class="line" id="l577">    577</a> 	}
<a href="#l578" class="line" id="l578">    578</a> }
<a href="#l579" class="line" id="l579">    579</a> 
<a href="#l580" class="line" id="l580">    580</a> void
<a href="#l581" class="line" id="l581">    581</a> configurerequest(XEvent *e)
<a href="#l582" class="line" id="l582">    582</a> {
<a href="#l583" class="line" id="l583">    583</a> 	Client *c;
<a href="#l584" class="line" id="l584">    584</a> 	Monitor *m;
<a href="#l585" class="line" id="l585">    585</a> 	XConfigureRequestEvent *ev = &amp;e-&gt;xconfigurerequest;
<a href="#l586" class="line" id="l586">    586</a> 	XWindowChanges wc;
<a href="#l587" class="line" id="l587">    587</a> 
<a href="#l588" class="line" id="l588">    588</a> 	if ((c = wintoclient(ev-&gt;window))) {
<a href="#l589" class="line" id="l589">    589</a> 		if (ev-&gt;value_mask &amp; CWBorderWidth)
<a href="#l590" class="line" id="l590">    590</a> 			c-&gt;bw = ev-&gt;border_width;
<a href="#l591" class="line" id="l591">    591</a> 		else if (c-&gt;isfloating || !selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange) {
<a href="#l592" class="line" id="l592">    592</a> 			m = c-&gt;mon;
<a href="#l593" class="line" id="l593">    593</a> 			if (ev-&gt;value_mask &amp; CWX) {
<a href="#l594" class="line" id="l594">    594</a> 				c-&gt;oldx = c-&gt;x;
<a href="#l595" class="line" id="l595">    595</a> 				c-&gt;x = m-&gt;mx + ev-&gt;x;
<a href="#l596" class="line" id="l596">    596</a> 			}
<a href="#l597" class="line" id="l597">    597</a> 			if (ev-&gt;value_mask &amp; CWY) {
<a href="#l598" class="line" id="l598">    598</a> 				c-&gt;oldy = c-&gt;y;
<a href="#l599" class="line" id="l599">    599</a> 				c-&gt;y = m-&gt;my + ev-&gt;y;
<a href="#l600" class="line" id="l600">    600</a> 			}
<a href="#l601" class="line" id="l601">    601</a> 			if (ev-&gt;value_mask &amp; CWWidth) {
<a href="#l602" class="line" id="l602">    602</a> 				c-&gt;oldw = c-&gt;w;
<a href="#l603" class="line" id="l603">    603</a> 				c-&gt;w = ev-&gt;width;
<a href="#l604" class="line" id="l604">    604</a> 			}
<a href="#l605" class="line" id="l605">    605</a> 			if (ev-&gt;value_mask &amp; CWHeight) {
<a href="#l606" class="line" id="l606">    606</a> 				c-&gt;oldh = c-&gt;h;
<a href="#l607" class="line" id="l607">    607</a> 				c-&gt;h = ev-&gt;height;
<a href="#l608" class="line" id="l608">    608</a> 			}
<a href="#l609" class="line" id="l609">    609</a> 			if ((c-&gt;x + c-&gt;w) &gt; m-&gt;mx + m-&gt;mw &amp;&amp; c-&gt;isfloating)
<a href="#l610" class="line" id="l610">    610</a> 				c-&gt;x = m-&gt;mx + (m-&gt;mw / 2 - WIDTH(c) / 2); /* center in x direction */
<a href="#l611" class="line" id="l611">    611</a> 			if ((c-&gt;y + c-&gt;h) &gt; m-&gt;my + m-&gt;mh &amp;&amp; c-&gt;isfloating)
<a href="#l612" class="line" id="l612">    612</a> 				c-&gt;y = m-&gt;my + (m-&gt;mh / 2 - HEIGHT(c) / 2); /* center in y direction */
<a href="#l613" class="line" id="l613">    613</a> 			if ((ev-&gt;value_mask &amp; (CWX|CWY)) &amp;&amp; !(ev-&gt;value_mask &amp; (CWWidth|CWHeight)))
<a href="#l614" class="line" id="l614">    614</a> 				configure(c);
<a href="#l615" class="line" id="l615">    615</a> 			if (ISVISIBLE(c))
<a href="#l616" class="line" id="l616">    616</a> 				XMoveResizeWindow(dpy, c-&gt;win, c-&gt;x, c-&gt;y, c-&gt;w, c-&gt;h);
<a href="#l617" class="line" id="l617">    617</a> 		} else
<a href="#l618" class="line" id="l618">    618</a> 			configure(c);
<a href="#l619" class="line" id="l619">    619</a> 	} else {
<a href="#l620" class="line" id="l620">    620</a> 		wc.x = ev-&gt;x;
<a href="#l621" class="line" id="l621">    621</a> 		wc.y = ev-&gt;y;
<a href="#l622" class="line" id="l622">    622</a> 		wc.width = ev-&gt;width;
<a href="#l623" class="line" id="l623">    623</a> 		wc.height = ev-&gt;height;
<a href="#l624" class="line" id="l624">    624</a> 		wc.border_width = ev-&gt;border_width;
<a href="#l625" class="line" id="l625">    625</a> 		wc.sibling = ev-&gt;above;
<a href="#l626" class="line" id="l626">    626</a> 		wc.stack_mode = ev-&gt;detail;
<a href="#l627" class="line" id="l627">    627</a> 		XConfigureWindow(dpy, ev-&gt;window, ev-&gt;value_mask, &amp;wc);
<a href="#l628" class="line" id="l628">    628</a> 	}
<a href="#l629" class="line" id="l629">    629</a> 	XSync(dpy, False);
<a href="#l630" class="line" id="l630">    630</a> }
<a href="#l631" class="line" id="l631">    631</a> 
<a href="#l632" class="line" id="l632">    632</a> Monitor *
<a href="#l633" class="line" id="l633">    633</a> createmon(void)
<a href="#l634" class="line" id="l634">    634</a> {
<a href="#l635" class="line" id="l635">    635</a> 	Monitor *m;
<a href="#l636" class="line" id="l636">    636</a> 
<a href="#l637" class="line" id="l637">    637</a> 	m = ecalloc(1, sizeof(Monitor));
<a href="#l638" class="line" id="l638">    638</a> 	m-&gt;tagset[0] = m-&gt;tagset[1] = 1;
<a href="#l639" class="line" id="l639">    639</a> 	m-&gt;mfact = mfact;
<a href="#l640" class="line" id="l640">    640</a> 	m-&gt;nmaster = nmaster;
<a href="#l641" class="line" id="l641">    641</a> 	m-&gt;showbar = showbar;
<a href="#l642" class="line" id="l642">    642</a> 	m-&gt;topbar = topbar;
<a href="#l643" class="line" id="l643">    643</a> 	m-&gt;lt[0] = &amp;layouts[0];
<a href="#l644" class="line" id="l644">    644</a> 	m-&gt;lt[1] = &amp;layouts[1 % LENGTH(layouts)];
<a href="#l645" class="line" id="l645">    645</a> 	strncpy(m-&gt;ltsymbol, layouts[0].symbol, sizeof m-&gt;ltsymbol);
<a href="#l646" class="line" id="l646">    646</a> 	return m;
<a href="#l647" class="line" id="l647">    647</a> }
<a href="#l648" class="line" id="l648">    648</a> 
<a href="#l649" class="line" id="l649">    649</a> void
<a href="#l650" class="line" id="l650">    650</a> destroynotify(XEvent *e)
<a href="#l651" class="line" id="l651">    651</a> {
<a href="#l652" class="line" id="l652">    652</a> 	Client *c;
<a href="#l653" class="line" id="l653">    653</a> 	XDestroyWindowEvent *ev = &amp;e-&gt;xdestroywindow;
<a href="#l654" class="line" id="l654">    654</a> 
<a href="#l655" class="line" id="l655">    655</a> 	if ((c = wintoclient(ev-&gt;window)))
<a href="#l656" class="line" id="l656">    656</a> 		unmanage(c, 1);
<a href="#l657" class="line" id="l657">    657</a> }
<a href="#l658" class="line" id="l658">    658</a> 
<a href="#l659" class="line" id="l659">    659</a> void
<a href="#l660" class="line" id="l660">    660</a> detach(Client *c)
<a href="#l661" class="line" id="l661">    661</a> {
<a href="#l662" class="line" id="l662">    662</a> 	Client **tc;
<a href="#l663" class="line" id="l663">    663</a> 
<a href="#l664" class="line" id="l664">    664</a> 	for (tc = &amp;c-&gt;mon-&gt;clients; *tc &amp;&amp; *tc != c; tc = &amp;(*tc)-&gt;next);
<a href="#l665" class="line" id="l665">    665</a> 	*tc = c-&gt;next;
<a href="#l666" class="line" id="l666">    666</a> }
<a href="#l667" class="line" id="l667">    667</a> 
<a href="#l668" class="line" id="l668">    668</a> void
<a href="#l669" class="line" id="l669">    669</a> detachstack(Client *c)
<a href="#l670" class="line" id="l670">    670</a> {
<a href="#l671" class="line" id="l671">    671</a> 	Client **tc, *t;
<a href="#l672" class="line" id="l672">    672</a> 
<a href="#l673" class="line" id="l673">    673</a> 	for (tc = &amp;c-&gt;mon-&gt;stack; *tc &amp;&amp; *tc != c; tc = &amp;(*tc)-&gt;snext);
<a href="#l674" class="line" id="l674">    674</a> 	*tc = c-&gt;snext;
<a href="#l675" class="line" id="l675">    675</a> 
<a href="#l676" class="line" id="l676">    676</a> 	if (c == c-&gt;mon-&gt;sel) {
<a href="#l677" class="line" id="l677">    677</a> 		for (t = c-&gt;mon-&gt;stack; t &amp;&amp; !ISVISIBLE(t); t = t-&gt;snext);
<a href="#l678" class="line" id="l678">    678</a> 		c-&gt;mon-&gt;sel = t;
<a href="#l679" class="line" id="l679">    679</a> 	}
<a href="#l680" class="line" id="l680">    680</a> }
<a href="#l681" class="line" id="l681">    681</a> 
<a href="#l682" class="line" id="l682">    682</a> Monitor *
<a href="#l683" class="line" id="l683">    683</a> dirtomon(int dir)
<a href="#l684" class="line" id="l684">    684</a> {
<a href="#l685" class="line" id="l685">    685</a> 	Monitor *m = NULL;
<a href="#l686" class="line" id="l686">    686</a> 
<a href="#l687" class="line" id="l687">    687</a> 	if (dir &gt; 0) {
<a href="#l688" class="line" id="l688">    688</a> 		if (!(m = selmon-&gt;next))
<a href="#l689" class="line" id="l689">    689</a> 			m = mons;
<a href="#l690" class="line" id="l690">    690</a> 	} else if (selmon == mons)
<a href="#l691" class="line" id="l691">    691</a> 		for (m = mons; m-&gt;next; m = m-&gt;next);
<a href="#l692" class="line" id="l692">    692</a> 	else
<a href="#l693" class="line" id="l693">    693</a> 		for (m = mons; m-&gt;next != selmon; m = m-&gt;next);
<a href="#l694" class="line" id="l694">    694</a> 	return m;
<a href="#l695" class="line" id="l695">    695</a> }
<a href="#l696" class="line" id="l696">    696</a> 
<a href="#l697" class="line" id="l697">    697</a> void
<a href="#l698" class="line" id="l698">    698</a> drawbar(Monitor *m)
<a href="#l699" class="line" id="l699">    699</a> {
<a href="#l700" class="line" id="l700">    700</a> 	int x, w, tw = 0;
<a href="#l701" class="line" id="l701">    701</a> 	int boxs = drw-&gt;fonts-&gt;h / 9;
<a href="#l702" class="line" id="l702">    702</a> 	int boxw = drw-&gt;fonts-&gt;h / 6 + 2;
<a href="#l703" class="line" id="l703">    703</a> 	unsigned int i, occ = 0, urg = 0;
<a href="#l704" class="line" id="l704">    704</a> 	Client *c;
<a href="#l705" class="line" id="l705">    705</a> 
<a href="#l706" class="line" id="l706">    706</a> 	if (!m-&gt;showbar)
<a href="#l707" class="line" id="l707">    707</a> 		return;
<a href="#l708" class="line" id="l708">    708</a> 
<a href="#l709" class="line" id="l709">    709</a> 	/* draw status first so it can be overdrawn by tags later */
<a href="#l710" class="line" id="l710">    710</a> 	if (m == selmon) { /* status is only drawn on selected monitor */
<a href="#l711" class="line" id="l711">    711</a> 		drw_setscheme(drw, scheme[SchemeNorm]);
<a href="#l712" class="line" id="l712">    712</a> 		tw = TEXTW(stext) - lrpad + 2; /* 2px right padding */
<a href="#l713" class="line" id="l713">    713</a> 		drw_text(drw, m-&gt;ww - tw, 0, tw, bh, 0, stext, 0);
<a href="#l714" class="line" id="l714">    714</a> 	}
<a href="#l715" class="line" id="l715">    715</a> 
<a href="#l716" class="line" id="l716">    716</a> 	for (c = m-&gt;clients; c; c = c-&gt;next) {
<a href="#l717" class="line" id="l717">    717</a> 		occ |= c-&gt;tags;
<a href="#l718" class="line" id="l718">    718</a> 		if (c-&gt;isurgent)
<a href="#l719" class="line" id="l719">    719</a> 			urg |= c-&gt;tags;
<a href="#l720" class="line" id="l720">    720</a> 	}
<a href="#l721" class="line" id="l721">    721</a> 	x = 0;
<a href="#l722" class="line" id="l722">    722</a> 	for (i = 0; i &lt; LENGTH(tags); i++) {
<a href="#l723" class="line" id="l723">    723</a> 		w = TEXTW(tags[i]);
<a href="#l724" class="line" id="l724">    724</a> 		drw_setscheme(drw, scheme[m-&gt;tagset[m-&gt;seltags] &amp; 1 &lt;&lt; i ? SchemeSel : SchemeNorm]);
<a href="#l725" class="line" id="l725">    725</a> 		drw_text(drw, x, 0, w, bh, lrpad / 2, tags[i], urg &amp; 1 &lt;&lt; i);
<a href="#l726" class="line" id="l726">    726</a> 		if (occ &amp; 1 &lt;&lt; i)
<a href="#l727" class="line" id="l727">    727</a> 			drw_rect(drw, x + boxs, boxs, boxw, boxw,
<a href="#l728" class="line" id="l728">    728</a> 				m == selmon &amp;&amp; selmon-&gt;sel &amp;&amp; selmon-&gt;sel-&gt;tags &amp; 1 &lt;&lt; i,
<a href="#l729" class="line" id="l729">    729</a> 				urg &amp; 1 &lt;&lt; i);
<a href="#l730" class="line" id="l730">    730</a> 		x += w;
<a href="#l731" class="line" id="l731">    731</a> 	}
<a href="#l732" class="line" id="l732">    732</a> 	w = TEXTW(m-&gt;ltsymbol);
<a href="#l733" class="line" id="l733">    733</a> 	drw_setscheme(drw, scheme[SchemeNorm]);
<a href="#l734" class="line" id="l734">    734</a> 	x = drw_text(drw, x, 0, w, bh, lrpad / 2, m-&gt;ltsymbol, 0);
<a href="#l735" class="line" id="l735">    735</a> 
<a href="#l736" class="line" id="l736">    736</a> 	if ((w = m-&gt;ww - tw - x) &gt; bh) {
<a href="#l737" class="line" id="l737">    737</a> 		if (m-&gt;sel) {
<a href="#l738" class="line" id="l738">    738</a> 			drw_setscheme(drw, scheme[m == selmon ? SchemeSel : SchemeNorm]);
<a href="#l739" class="line" id="l739">    739</a> 			drw_text(drw, x, 0, w, bh, lrpad / 2, m-&gt;sel-&gt;name, 0);
<a href="#l740" class="line" id="l740">    740</a> 			if (m-&gt;sel-&gt;isfloating)
<a href="#l741" class="line" id="l741">    741</a> 				drw_rect(drw, x + boxs, boxs, boxw, boxw, m-&gt;sel-&gt;isfixed, 0);
<a href="#l742" class="line" id="l742">    742</a> 		} else {
<a href="#l743" class="line" id="l743">    743</a> 			drw_setscheme(drw, scheme[SchemeNorm]);
<a href="#l744" class="line" id="l744">    744</a> 			drw_rect(drw, x, 0, w, bh, 1, 1);
<a href="#l745" class="line" id="l745">    745</a> 		}
<a href="#l746" class="line" id="l746">    746</a> 	}
<a href="#l747" class="line" id="l747">    747</a> 	drw_map(drw, m-&gt;barwin, 0, 0, m-&gt;ww, bh);
<a href="#l748" class="line" id="l748">    748</a> }
<a href="#l749" class="line" id="l749">    749</a> 
<a href="#l750" class="line" id="l750">    750</a> void
<a href="#l751" class="line" id="l751">    751</a> drawbars(void)
<a href="#l752" class="line" id="l752">    752</a> {
<a href="#l753" class="line" id="l753">    753</a> 	Monitor *m;
<a href="#l754" class="line" id="l754">    754</a> 
<a href="#l755" class="line" id="l755">    755</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l756" class="line" id="l756">    756</a> 		drawbar(m);
<a href="#l757" class="line" id="l757">    757</a> }
<a href="#l758" class="line" id="l758">    758</a> 
<a href="#l759" class="line" id="l759">    759</a> void
<a href="#l760" class="line" id="l760">    760</a> enternotify(XEvent *e)
<a href="#l761" class="line" id="l761">    761</a> {
<a href="#l762" class="line" id="l762">    762</a> 	Client *c;
<a href="#l763" class="line" id="l763">    763</a> 	Monitor *m;
<a href="#l764" class="line" id="l764">    764</a> 	XCrossingEvent *ev = &amp;e-&gt;xcrossing;
<a href="#l765" class="line" id="l765">    765</a> 
<a href="#l766" class="line" id="l766">    766</a> 	if ((ev-&gt;mode != NotifyNormal || ev-&gt;detail == NotifyInferior) &amp;&amp; ev-&gt;window != root)
<a href="#l767" class="line" id="l767">    767</a> 		return;
<a href="#l768" class="line" id="l768">    768</a> 	c = wintoclient(ev-&gt;window);
<a href="#l769" class="line" id="l769">    769</a> 	m = c ? c-&gt;mon : wintomon(ev-&gt;window);
<a href="#l770" class="line" id="l770">    770</a> 	if (m != selmon) {
<a href="#l771" class="line" id="l771">    771</a> 		unfocus(selmon-&gt;sel, 1);
<a href="#l772" class="line" id="l772">    772</a> 		selmon = m;
<a href="#l773" class="line" id="l773">    773</a> 	} else if (!c || c == selmon-&gt;sel)
<a href="#l774" class="line" id="l774">    774</a> 		return;
<a href="#l775" class="line" id="l775">    775</a> 	focus(c);
<a href="#l776" class="line" id="l776">    776</a> }
<a href="#l777" class="line" id="l777">    777</a> 
<a href="#l778" class="line" id="l778">    778</a> void
<a href="#l779" class="line" id="l779">    779</a> expose(XEvent *e)
<a href="#l780" class="line" id="l780">    780</a> {
<a href="#l781" class="line" id="l781">    781</a> 	Monitor *m;
<a href="#l782" class="line" id="l782">    782</a> 	XExposeEvent *ev = &amp;e-&gt;xexpose;
<a href="#l783" class="line" id="l783">    783</a> 
<a href="#l784" class="line" id="l784">    784</a> 	if (ev-&gt;count == 0 &amp;&amp; (m = wintomon(ev-&gt;window)))
<a href="#l785" class="line" id="l785">    785</a> 		drawbar(m);
<a href="#l786" class="line" id="l786">    786</a> }
<a href="#l787" class="line" id="l787">    787</a> 
<a href="#l788" class="line" id="l788">    788</a> void
<a href="#l789" class="line" id="l789">    789</a> focus(Client *c)
<a href="#l790" class="line" id="l790">    790</a> {
<a href="#l791" class="line" id="l791">    791</a> 	if (!c || !ISVISIBLE(c))
<a href="#l792" class="line" id="l792">    792</a> 		for (c = selmon-&gt;stack; c &amp;&amp; !ISVISIBLE(c); c = c-&gt;snext);
<a href="#l793" class="line" id="l793">    793</a> 	if (selmon-&gt;sel &amp;&amp; selmon-&gt;sel != c)
<a href="#l794" class="line" id="l794">    794</a> 		unfocus(selmon-&gt;sel, 0);
<a href="#l795" class="line" id="l795">    795</a> 	if (c) {
<a href="#l796" class="line" id="l796">    796</a> 		if (c-&gt;mon != selmon)
<a href="#l797" class="line" id="l797">    797</a> 			selmon = c-&gt;mon;
<a href="#l798" class="line" id="l798">    798</a> 		if (c-&gt;isurgent)
<a href="#l799" class="line" id="l799">    799</a> 			seturgent(c, 0);
<a href="#l800" class="line" id="l800">    800</a> 		detachstack(c);
<a href="#l801" class="line" id="l801">    801</a> 		attachstack(c);
<a href="#l802" class="line" id="l802">    802</a> 		grabbuttons(c, 1);
<a href="#l803" class="line" id="l803">    803</a> 		XSetWindowBorder(dpy, c-&gt;win, scheme[SchemeSel][ColBorder].pixel);
<a href="#l804" class="line" id="l804">    804</a> 		setfocus(c);
<a href="#l805" class="line" id="l805">    805</a> 	} else {
<a href="#l806" class="line" id="l806">    806</a> 		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
<a href="#l807" class="line" id="l807">    807</a> 		XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
<a href="#l808" class="line" id="l808">    808</a> 	}
<a href="#l809" class="line" id="l809">    809</a> 	selmon-&gt;sel = c;
<a href="#l810" class="line" id="l810">    810</a> 	drawbars();
<a href="#l811" class="line" id="l811">    811</a> }
<a href="#l812" class="line" id="l812">    812</a> 
<a href="#l813" class="line" id="l813">    813</a> /* there are some broken focus acquiring clients needing extra handling */
<a href="#l814" class="line" id="l814">    814</a> void
<a href="#l815" class="line" id="l815">    815</a> focusin(XEvent *e)
<a href="#l816" class="line" id="l816">    816</a> {
<a href="#l817" class="line" id="l817">    817</a> 	XFocusChangeEvent *ev = &amp;e-&gt;xfocus;
<a href="#l818" class="line" id="l818">    818</a> 
<a href="#l819" class="line" id="l819">    819</a> 	if (selmon-&gt;sel &amp;&amp; ev-&gt;window != selmon-&gt;sel-&gt;win)
<a href="#l820" class="line" id="l820">    820</a> 		setfocus(selmon-&gt;sel);
<a href="#l821" class="line" id="l821">    821</a> }
<a href="#l822" class="line" id="l822">    822</a> 
<a href="#l823" class="line" id="l823">    823</a> void
<a href="#l824" class="line" id="l824">    824</a> focusmon(const Arg *arg)
<a href="#l825" class="line" id="l825">    825</a> {
<a href="#l826" class="line" id="l826">    826</a> 	Monitor *m;
<a href="#l827" class="line" id="l827">    827</a> 
<a href="#l828" class="line" id="l828">    828</a> 	if (!mons-&gt;next)
<a href="#l829" class="line" id="l829">    829</a> 		return;
<a href="#l830" class="line" id="l830">    830</a> 	if ((m = dirtomon(arg-&gt;i)) == selmon)
<a href="#l831" class="line" id="l831">    831</a> 		return;
<a href="#l832" class="line" id="l832">    832</a> 	unfocus(selmon-&gt;sel, 0);
<a href="#l833" class="line" id="l833">    833</a> 	selmon = m;
<a href="#l834" class="line" id="l834">    834</a> 	focus(NULL);
<a href="#l835" class="line" id="l835">    835</a> }
<a href="#l836" class="line" id="l836">    836</a> 
<a href="#l837" class="line" id="l837">    837</a> void
<a href="#l838" class="line" id="l838">    838</a> focusstack(const Arg *arg)
<a href="#l839" class="line" id="l839">    839</a> {
<a href="#l840" class="line" id="l840">    840</a> 	Client *c = NULL, *i;
<a href="#l841" class="line" id="l841">    841</a> 
<a href="#l842" class="line" id="l842">    842</a> 	if (!selmon-&gt;sel || (selmon-&gt;sel-&gt;isfullscreen &amp;&amp; lockfullscreen))
<a href="#l843" class="line" id="l843">    843</a> 		return;
<a href="#l844" class="line" id="l844">    844</a> 	if (arg-&gt;i &gt; 0) {
<a href="#l845" class="line" id="l845">    845</a> 		for (c = selmon-&gt;sel-&gt;next; c &amp;&amp; !ISVISIBLE(c); c = c-&gt;next);
<a href="#l846" class="line" id="l846">    846</a> 		if (!c)
<a href="#l847" class="line" id="l847">    847</a> 			for (c = selmon-&gt;clients; c &amp;&amp; !ISVISIBLE(c); c = c-&gt;next);
<a href="#l848" class="line" id="l848">    848</a> 	} else {
<a href="#l849" class="line" id="l849">    849</a> 		for (i = selmon-&gt;clients; i != selmon-&gt;sel; i = i-&gt;next)
<a href="#l850" class="line" id="l850">    850</a> 			if (ISVISIBLE(i))
<a href="#l851" class="line" id="l851">    851</a> 				c = i;
<a href="#l852" class="line" id="l852">    852</a> 		if (!c)
<a href="#l853" class="line" id="l853">    853</a> 			for (; i; i = i-&gt;next)
<a href="#l854" class="line" id="l854">    854</a> 				if (ISVISIBLE(i))
<a href="#l855" class="line" id="l855">    855</a> 					c = i;
<a href="#l856" class="line" id="l856">    856</a> 	}
<a href="#l857" class="line" id="l857">    857</a> 	if (c) {
<a href="#l858" class="line" id="l858">    858</a> 		focus(c);
<a href="#l859" class="line" id="l859">    859</a> 		restack(selmon);
<a href="#l860" class="line" id="l860">    860</a> 	}
<a href="#l861" class="line" id="l861">    861</a> }
<a href="#l862" class="line" id="l862">    862</a> 
<a href="#l863" class="line" id="l863">    863</a> Atom
<a href="#l864" class="line" id="l864">    864</a> getatomprop(Client *c, Atom prop)
<a href="#l865" class="line" id="l865">    865</a> {
<a href="#l866" class="line" id="l866">    866</a> 	int di;
<a href="#l867" class="line" id="l867">    867</a> 	unsigned long dl;
<a href="#l868" class="line" id="l868">    868</a> 	unsigned char *p = NULL;
<a href="#l869" class="line" id="l869">    869</a> 	Atom da, atom = None;
<a href="#l870" class="line" id="l870">    870</a> 
<a href="#l871" class="line" id="l871">    871</a> 	if (XGetWindowProperty(dpy, c-&gt;win, prop, 0L, sizeof atom, False, XA_ATOM,
<a href="#l872" class="line" id="l872">    872</a> 		&amp;da, &amp;di, &amp;dl, &amp;dl, &amp;p) == Success &amp;&amp; p) {
<a href="#l873" class="line" id="l873">    873</a> 		atom = *(Atom *)p;
<a href="#l874" class="line" id="l874">    874</a> 		XFree(p);
<a href="#l875" class="line" id="l875">    875</a> 	}
<a href="#l876" class="line" id="l876">    876</a> 	return atom;
<a href="#l877" class="line" id="l877">    877</a> }
<a href="#l878" class="line" id="l878">    878</a> 
<a href="#l879" class="line" id="l879">    879</a> int
<a href="#l880" class="line" id="l880">    880</a> getrootptr(int *x, int *y)
<a href="#l881" class="line" id="l881">    881</a> {
<a href="#l882" class="line" id="l882">    882</a> 	int di;
<a href="#l883" class="line" id="l883">    883</a> 	unsigned int dui;
<a href="#l884" class="line" id="l884">    884</a> 	Window dummy;
<a href="#l885" class="line" id="l885">    885</a> 
<a href="#l886" class="line" id="l886">    886</a> 	return XQueryPointer(dpy, root, &amp;dummy, &amp;dummy, x, y, &amp;di, &amp;di, &amp;dui);
<a href="#l887" class="line" id="l887">    887</a> }
<a href="#l888" class="line" id="l888">    888</a> 
<a href="#l889" class="line" id="l889">    889</a> long
<a href="#l890" class="line" id="l890">    890</a> getstate(Window w)
<a href="#l891" class="line" id="l891">    891</a> {
<a href="#l892" class="line" id="l892">    892</a> 	int format;
<a href="#l893" class="line" id="l893">    893</a> 	long result = -1;
<a href="#l894" class="line" id="l894">    894</a> 	unsigned char *p = NULL;
<a href="#l895" class="line" id="l895">    895</a> 	unsigned long n, extra;
<a href="#l896" class="line" id="l896">    896</a> 	Atom real;
<a href="#l897" class="line" id="l897">    897</a> 
<a href="#l898" class="line" id="l898">    898</a> 	if (XGetWindowProperty(dpy, w, wmatom[WMState], 0L, 2L, False, wmatom[WMState],
<a href="#l899" class="line" id="l899">    899</a> 		&amp;real, &amp;format, &amp;n, &amp;extra, (unsigned char **)&amp;p) != Success)
<a href="#l900" class="line" id="l900">    900</a> 		return -1;
<a href="#l901" class="line" id="l901">    901</a> 	if (n != 0)
<a href="#l902" class="line" id="l902">    902</a> 		result = *p;
<a href="#l903" class="line" id="l903">    903</a> 	XFree(p);
<a href="#l904" class="line" id="l904">    904</a> 	return result;
<a href="#l905" class="line" id="l905">    905</a> }
<a href="#l906" class="line" id="l906">    906</a> 
<a href="#l907" class="line" id="l907">    907</a> int
<a href="#l908" class="line" id="l908">    908</a> gettextprop(Window w, Atom atom, char *text, unsigned int size)
<a href="#l909" class="line" id="l909">    909</a> {
<a href="#l910" class="line" id="l910">    910</a> 	char **list = NULL;
<a href="#l911" class="line" id="l911">    911</a> 	int n;
<a href="#l912" class="line" id="l912">    912</a> 	XTextProperty name;
<a href="#l913" class="line" id="l913">    913</a> 
<a href="#l914" class="line" id="l914">    914</a> 	if (!text || size == 0)
<a href="#l915" class="line" id="l915">    915</a> 		return 0;
<a href="#l916" class="line" id="l916">    916</a> 	text[0] = &#39;\0&#39;;
<a href="#l917" class="line" id="l917">    917</a> 	if (!XGetTextProperty(dpy, w, &amp;name, atom) || !name.nitems)
<a href="#l918" class="line" id="l918">    918</a> 		return 0;
<a href="#l919" class="line" id="l919">    919</a> 	if (name.encoding == XA_STRING) {
<a href="#l920" class="line" id="l920">    920</a> 		strncpy(text, (char *)name.value, size - 1);
<a href="#l921" class="line" id="l921">    921</a> 	} else if (XmbTextPropertyToTextList(dpy, &amp;name, &amp;list, &amp;n) &gt;= Success &amp;&amp; n &gt; 0 &amp;&amp; *list) {
<a href="#l922" class="line" id="l922">    922</a> 		strncpy(text, *list, size - 1);
<a href="#l923" class="line" id="l923">    923</a> 		XFreeStringList(list);
<a href="#l924" class="line" id="l924">    924</a> 	}
<a href="#l925" class="line" id="l925">    925</a> 	text[size - 1] = &#39;\0&#39;;
<a href="#l926" class="line" id="l926">    926</a> 	XFree(name.value);
<a href="#l927" class="line" id="l927">    927</a> 	return 1;
<a href="#l928" class="line" id="l928">    928</a> }
<a href="#l929" class="line" id="l929">    929</a> 
<a href="#l930" class="line" id="l930">    930</a> void
<a href="#l931" class="line" id="l931">    931</a> grabbuttons(Client *c, int focused)
<a href="#l932" class="line" id="l932">    932</a> {
<a href="#l933" class="line" id="l933">    933</a> 	updatenumlockmask();
<a href="#l934" class="line" id="l934">    934</a> 	{
<a href="#l935" class="line" id="l935">    935</a> 		unsigned int i, j;
<a href="#l936" class="line" id="l936">    936</a> 		unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
<a href="#l937" class="line" id="l937">    937</a> 		XUngrabButton(dpy, AnyButton, AnyModifier, c-&gt;win);
<a href="#l938" class="line" id="l938">    938</a> 		if (!focused)
<a href="#l939" class="line" id="l939">    939</a> 			XGrabButton(dpy, AnyButton, AnyModifier, c-&gt;win, False,
<a href="#l940" class="line" id="l940">    940</a> 				BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
<a href="#l941" class="line" id="l941">    941</a> 		for (i = 0; i &lt; LENGTH(buttons); i++)
<a href="#l942" class="line" id="l942">    942</a> 			if (buttons[i].click == ClkClientWin)
<a href="#l943" class="line" id="l943">    943</a> 				for (j = 0; j &lt; LENGTH(modifiers); j++)
<a href="#l944" class="line" id="l944">    944</a> 					XGrabButton(dpy, buttons[i].button,
<a href="#l945" class="line" id="l945">    945</a> 						buttons[i].mask | modifiers[j],
<a href="#l946" class="line" id="l946">    946</a> 						c-&gt;win, False, BUTTONMASK,
<a href="#l947" class="line" id="l947">    947</a> 						GrabModeAsync, GrabModeSync, None, None);
<a href="#l948" class="line" id="l948">    948</a> 	}
<a href="#l949" class="line" id="l949">    949</a> }
<a href="#l950" class="line" id="l950">    950</a> 
<a href="#l951" class="line" id="l951">    951</a> void
<a href="#l952" class="line" id="l952">    952</a> grabkeys(void)
<a href="#l953" class="line" id="l953">    953</a> {
<a href="#l954" class="line" id="l954">    954</a> 	updatenumlockmask();
<a href="#l955" class="line" id="l955">    955</a> 	{
<a href="#l956" class="line" id="l956">    956</a> 		unsigned int i, j, k;
<a href="#l957" class="line" id="l957">    957</a> 		unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
<a href="#l958" class="line" id="l958">    958</a> 		int start, end, skip;
<a href="#l959" class="line" id="l959">    959</a> 		KeySym *syms;
<a href="#l960" class="line" id="l960">    960</a> 
<a href="#l961" class="line" id="l961">    961</a> 		XUngrabKey(dpy, AnyKey, AnyModifier, root);
<a href="#l962" class="line" id="l962">    962</a> 		XDisplayKeycodes(dpy, &amp;start, &amp;end);
<a href="#l963" class="line" id="l963">    963</a> 		syms = XGetKeyboardMapping(dpy, start, end - start + 1, &amp;skip);
<a href="#l964" class="line" id="l964">    964</a> 		if (!syms)
<a href="#l965" class="line" id="l965">    965</a> 			return;
<a href="#l966" class="line" id="l966">    966</a> 		for (k = start; k &lt;= end; k++)
<a href="#l967" class="line" id="l967">    967</a> 			for (i = 0; i &lt; LENGTH(keys); i++)
<a href="#l968" class="line" id="l968">    968</a> 				/* skip modifier codes, we do that ourselves */
<a href="#l969" class="line" id="l969">    969</a> 				if (keys[i].keysym == syms[(k - start) * skip])
<a href="#l970" class="line" id="l970">    970</a> 					for (j = 0; j &lt; LENGTH(modifiers); j++)
<a href="#l971" class="line" id="l971">    971</a> 						XGrabKey(dpy, k,
<a href="#l972" class="line" id="l972">    972</a> 							 keys[i].mod | modifiers[j],
<a href="#l973" class="line" id="l973">    973</a> 							 root, True,
<a href="#l974" class="line" id="l974">    974</a> 							 GrabModeAsync, GrabModeAsync);
<a href="#l975" class="line" id="l975">    975</a> 		XFree(syms);
<a href="#l976" class="line" id="l976">    976</a> 	}
<a href="#l977" class="line" id="l977">    977</a> }
<a href="#l978" class="line" id="l978">    978</a> 
<a href="#l979" class="line" id="l979">    979</a> void
<a href="#l980" class="line" id="l980">    980</a> incnmaster(const Arg *arg)
<a href="#l981" class="line" id="l981">    981</a> {
<a href="#l982" class="line" id="l982">    982</a> 	selmon-&gt;nmaster = MAX(selmon-&gt;nmaster + arg-&gt;i, 0);
<a href="#l983" class="line" id="l983">    983</a> 	arrange(selmon);
<a href="#l984" class="line" id="l984">    984</a> }
<a href="#l985" class="line" id="l985">    985</a> 
<a href="#l986" class="line" id="l986">    986</a> #ifdef XINERAMA
<a href="#l987" class="line" id="l987">    987</a> static int
<a href="#l988" class="line" id="l988">    988</a> isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
<a href="#l989" class="line" id="l989">    989</a> {
<a href="#l990" class="line" id="l990">    990</a> 	while (n--)
<a href="#l991" class="line" id="l991">    991</a> 		if (unique[n].x_org == info-&gt;x_org &amp;&amp; unique[n].y_org == info-&gt;y_org
<a href="#l992" class="line" id="l992">    992</a> 		&amp;&amp; unique[n].width == info-&gt;width &amp;&amp; unique[n].height == info-&gt;height)
<a href="#l993" class="line" id="l993">    993</a> 			return 0;
<a href="#l994" class="line" id="l994">    994</a> 	return 1;
<a href="#l995" class="line" id="l995">    995</a> }
<a href="#l996" class="line" id="l996">    996</a> #endif /* XINERAMA */
<a href="#l997" class="line" id="l997">    997</a> 
<a href="#l998" class="line" id="l998">    998</a> void
<a href="#l999" class="line" id="l999">    999</a> keypress(XEvent *e)
<a href="#l1000" class="line" id="l1000">   1000</a> {
<a href="#l1001" class="line" id="l1001">   1001</a> 	unsigned int i;
<a href="#l1002" class="line" id="l1002">   1002</a> 	KeySym keysym;
<a href="#l1003" class="line" id="l1003">   1003</a> 	XKeyEvent *ev;
<a href="#l1004" class="line" id="l1004">   1004</a> 
<a href="#l1005" class="line" id="l1005">   1005</a> 	ev = &amp;e-&gt;xkey;
<a href="#l1006" class="line" id="l1006">   1006</a> 	keysym = XKeycodeToKeysym(dpy, (KeyCode)ev-&gt;keycode, 0);
<a href="#l1007" class="line" id="l1007">   1007</a> 	for (i = 0; i &lt; LENGTH(keys); i++)
<a href="#l1008" class="line" id="l1008">   1008</a> 		if (keysym == keys[i].keysym
<a href="#l1009" class="line" id="l1009">   1009</a> 		&amp;&amp; CLEANMASK(keys[i].mod) == CLEANMASK(ev-&gt;state)
<a href="#l1010" class="line" id="l1010">   1010</a> 		&amp;&amp; keys[i].func)
<a href="#l1011" class="line" id="l1011">   1011</a> 			keys[i].func(&amp;(keys[i].arg));
<a href="#l1012" class="line" id="l1012">   1012</a> }
<a href="#l1013" class="line" id="l1013">   1013</a> 
<a href="#l1014" class="line" id="l1014">   1014</a> void
<a href="#l1015" class="line" id="l1015">   1015</a> killclient(const Arg *arg)
<a href="#l1016" class="line" id="l1016">   1016</a> {
<a href="#l1017" class="line" id="l1017">   1017</a> 	if (!selmon-&gt;sel)
<a href="#l1018" class="line" id="l1018">   1018</a> 		return;
<a href="#l1019" class="line" id="l1019">   1019</a> 	if (!sendevent(selmon-&gt;sel, wmatom[WMDelete])) {
<a href="#l1020" class="line" id="l1020">   1020</a> 		XGrabServer(dpy);
<a href="#l1021" class="line" id="l1021">   1021</a> 		XSetErrorHandler(xerrordummy);
<a href="#l1022" class="line" id="l1022">   1022</a> 		XSetCloseDownMode(dpy, DestroyAll);
<a href="#l1023" class="line" id="l1023">   1023</a> 		XKillClient(dpy, selmon-&gt;sel-&gt;win);
<a href="#l1024" class="line" id="l1024">   1024</a> 		XSync(dpy, False);
<a href="#l1025" class="line" id="l1025">   1025</a> 		XSetErrorHandler(xerror);
<a href="#l1026" class="line" id="l1026">   1026</a> 		XUngrabServer(dpy);
<a href="#l1027" class="line" id="l1027">   1027</a> 	}
<a href="#l1028" class="line" id="l1028">   1028</a> }
<a href="#l1029" class="line" id="l1029">   1029</a> 
<a href="#l1030" class="line" id="l1030">   1030</a> void
<a href="#l1031" class="line" id="l1031">   1031</a> manage(Window w, XWindowAttributes *wa)
<a href="#l1032" class="line" id="l1032">   1032</a> {
<a href="#l1033" class="line" id="l1033">   1033</a> 	Client *c, *t = NULL;
<a href="#l1034" class="line" id="l1034">   1034</a> 	Window trans = None;
<a href="#l1035" class="line" id="l1035">   1035</a> 	XWindowChanges wc;
<a href="#l1036" class="line" id="l1036">   1036</a> 
<a href="#l1037" class="line" id="l1037">   1037</a> 	c = ecalloc(1, sizeof(Client));
<a href="#l1038" class="line" id="l1038">   1038</a> 	c-&gt;win = w;
<a href="#l1039" class="line" id="l1039">   1039</a> 	/* geometry */
<a href="#l1040" class="line" id="l1040">   1040</a> 	c-&gt;x = c-&gt;oldx = wa-&gt;x;
<a href="#l1041" class="line" id="l1041">   1041</a> 	c-&gt;y = c-&gt;oldy = wa-&gt;y;
<a href="#l1042" class="line" id="l1042">   1042</a> 	c-&gt;w = c-&gt;oldw = wa-&gt;width;
<a href="#l1043" class="line" id="l1043">   1043</a> 	c-&gt;h = c-&gt;oldh = wa-&gt;height;
<a href="#l1044" class="line" id="l1044">   1044</a> 	c-&gt;oldbw = wa-&gt;border_width;
<a href="#l1045" class="line" id="l1045">   1045</a> 
<a href="#l1046" class="line" id="l1046">   1046</a> 	updatetitle(c);
<a href="#l1047" class="line" id="l1047">   1047</a> 	if (XGetTransientForHint(dpy, w, &amp;trans) &amp;&amp; (t = wintoclient(trans))) {
<a href="#l1048" class="line" id="l1048">   1048</a> 		c-&gt;mon = t-&gt;mon;
<a href="#l1049" class="line" id="l1049">   1049</a> 		c-&gt;tags = t-&gt;tags;
<a href="#l1050" class="line" id="l1050">   1050</a> 	} else {
<a href="#l1051" class="line" id="l1051">   1051</a> 		c-&gt;mon = selmon;
<a href="#l1052" class="line" id="l1052">   1052</a> 		applyrules(c);
<a href="#l1053" class="line" id="l1053">   1053</a> 	}
<a href="#l1054" class="line" id="l1054">   1054</a> 
<a href="#l1055" class="line" id="l1055">   1055</a> 	if (c-&gt;x + WIDTH(c) &gt; c-&gt;mon-&gt;wx + c-&gt;mon-&gt;ww)
<a href="#l1056" class="line" id="l1056">   1056</a> 		c-&gt;x = c-&gt;mon-&gt;wx + c-&gt;mon-&gt;ww - WIDTH(c);
<a href="#l1057" class="line" id="l1057">   1057</a> 	if (c-&gt;y + HEIGHT(c) &gt; c-&gt;mon-&gt;wy + c-&gt;mon-&gt;wh)
<a href="#l1058" class="line" id="l1058">   1058</a> 		c-&gt;y = c-&gt;mon-&gt;wy + c-&gt;mon-&gt;wh - HEIGHT(c);
<a href="#l1059" class="line" id="l1059">   1059</a> 	c-&gt;x = MAX(c-&gt;x, c-&gt;mon-&gt;wx);
<a href="#l1060" class="line" id="l1060">   1060</a> 	c-&gt;y = MAX(c-&gt;y, c-&gt;mon-&gt;wy);
<a href="#l1061" class="line" id="l1061">   1061</a> 	c-&gt;bw = borderpx;
<a href="#l1062" class="line" id="l1062">   1062</a> 
<a href="#l1063" class="line" id="l1063">   1063</a> 	wc.border_width = c-&gt;bw;
<a href="#l1064" class="line" id="l1064">   1064</a> 	XConfigureWindow(dpy, w, CWBorderWidth, &amp;wc);
<a href="#l1065" class="line" id="l1065">   1065</a> 	XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColBorder].pixel);
<a href="#l1066" class="line" id="l1066">   1066</a> 	configure(c); /* propagates border_width, if size doesn&#39;t change */
<a href="#l1067" class="line" id="l1067">   1067</a> 	updatewindowtype(c);
<a href="#l1068" class="line" id="l1068">   1068</a> 	updatesizehints(c);
<a href="#l1069" class="line" id="l1069">   1069</a> 	updatewmhints(c);
<a href="#l1070" class="line" id="l1070">   1070</a> 	XSelectInput(dpy, w, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
<a href="#l1071" class="line" id="l1071">   1071</a> 	grabbuttons(c, 0);
<a href="#l1072" class="line" id="l1072">   1072</a> 	if (!c-&gt;isfloating)
<a href="#l1073" class="line" id="l1073">   1073</a> 		c-&gt;isfloating = c-&gt;oldstate = trans != None || c-&gt;isfixed;
<a href="#l1074" class="line" id="l1074">   1074</a> 	if (c-&gt;isfloating)
<a href="#l1075" class="line" id="l1075">   1075</a> 		XRaiseWindow(dpy, c-&gt;win);
<a href="#l1076" class="line" id="l1076">   1076</a> 	attach(c);
<a href="#l1077" class="line" id="l1077">   1077</a> 	attachstack(c);
<a href="#l1078" class="line" id="l1078">   1078</a> 	XChangeProperty(dpy, root, netatom[NetClientList], XA_WINDOW, 32, PropModeAppend,
<a href="#l1079" class="line" id="l1079">   1079</a> 		(unsigned char *) &amp;(c-&gt;win), 1);
<a href="#l1080" class="line" id="l1080">   1080</a> 	XMoveResizeWindow(dpy, c-&gt;win, c-&gt;x + 2 * sw, c-&gt;y, c-&gt;w, c-&gt;h); /* some windows require this */
<a href="#l1081" class="line" id="l1081">   1081</a> 	setclientstate(c, NormalState);
<a href="#l1082" class="line" id="l1082">   1082</a> 	if (c-&gt;mon == selmon)
<a href="#l1083" class="line" id="l1083">   1083</a> 		unfocus(selmon-&gt;sel, 0);
<a href="#l1084" class="line" id="l1084">   1084</a> 	c-&gt;mon-&gt;sel = c;
<a href="#l1085" class="line" id="l1085">   1085</a> 	arrange(c-&gt;mon);
<a href="#l1086" class="line" id="l1086">   1086</a> 	XMapWindow(dpy, c-&gt;win);
<a href="#l1087" class="line" id="l1087">   1087</a> 	focus(NULL);
<a href="#l1088" class="line" id="l1088">   1088</a> }
<a href="#l1089" class="line" id="l1089">   1089</a> 
<a href="#l1090" class="line" id="l1090">   1090</a> void
<a href="#l1091" class="line" id="l1091">   1091</a> mappingnotify(XEvent *e)
<a href="#l1092" class="line" id="l1092">   1092</a> {
<a href="#l1093" class="line" id="l1093">   1093</a> 	XMappingEvent *ev = &amp;e-&gt;xmapping;
<a href="#l1094" class="line" id="l1094">   1094</a> 
<a href="#l1095" class="line" id="l1095">   1095</a> 	XRefreshKeyboardMapping(ev);
<a href="#l1096" class="line" id="l1096">   1096</a> 	if (ev-&gt;request == MappingKeyboard)
<a href="#l1097" class="line" id="l1097">   1097</a> 		grabkeys();
<a href="#l1098" class="line" id="l1098">   1098</a> }
<a href="#l1099" class="line" id="l1099">   1099</a> 
<a href="#l1100" class="line" id="l1100">   1100</a> void
<a href="#l1101" class="line" id="l1101">   1101</a> maprequest(XEvent *e)
<a href="#l1102" class="line" id="l1102">   1102</a> {
<a href="#l1103" class="line" id="l1103">   1103</a> 	static XWindowAttributes wa;
<a href="#l1104" class="line" id="l1104">   1104</a> 	XMapRequestEvent *ev = &amp;e-&gt;xmaprequest;
<a href="#l1105" class="line" id="l1105">   1105</a> 
<a href="#l1106" class="line" id="l1106">   1106</a> 	if (!XGetWindowAttributes(dpy, ev-&gt;window, &amp;wa) || wa.override_redirect)
<a href="#l1107" class="line" id="l1107">   1107</a> 		return;
<a href="#l1108" class="line" id="l1108">   1108</a> 	if (!wintoclient(ev-&gt;window))
<a href="#l1109" class="line" id="l1109">   1109</a> 		manage(ev-&gt;window, &amp;wa);
<a href="#l1110" class="line" id="l1110">   1110</a> }
<a href="#l1111" class="line" id="l1111">   1111</a> 
<a href="#l1112" class="line" id="l1112">   1112</a> void
<a href="#l1113" class="line" id="l1113">   1113</a> monocle(Monitor *m)
<a href="#l1114" class="line" id="l1114">   1114</a> {
<a href="#l1115" class="line" id="l1115">   1115</a> 	unsigned int n = 0;
<a href="#l1116" class="line" id="l1116">   1116</a> 	Client *c;
<a href="#l1117" class="line" id="l1117">   1117</a> 
<a href="#l1118" class="line" id="l1118">   1118</a> 	for (c = m-&gt;clients; c; c = c-&gt;next)
<a href="#l1119" class="line" id="l1119">   1119</a> 		if (ISVISIBLE(c))
<a href="#l1120" class="line" id="l1120">   1120</a> 			n++;
<a href="#l1121" class="line" id="l1121">   1121</a> 	if (n &gt; 0) /* override layout symbol */
<a href="#l1122" class="line" id="l1122">   1122</a> 		snprintf(m-&gt;ltsymbol, sizeof m-&gt;ltsymbol, &quot;[%d]&quot;, n);
<a href="#l1123" class="line" id="l1123">   1123</a> 	for (c = nexttiled(m-&gt;clients); c; c = nexttiled(c-&gt;next))
<a href="#l1124" class="line" id="l1124">   1124</a> 		resize(c, m-&gt;wx, m-&gt;wy, m-&gt;ww - 2 * c-&gt;bw, m-&gt;wh - 2 * c-&gt;bw, 0);
<a href="#l1125" class="line" id="l1125">   1125</a> }
<a href="#l1126" class="line" id="l1126">   1126</a> 
<a href="#l1127" class="line" id="l1127">   1127</a> void
<a href="#l1128" class="line" id="l1128">   1128</a> motionnotify(XEvent *e)
<a href="#l1129" class="line" id="l1129">   1129</a> {
<a href="#l1130" class="line" id="l1130">   1130</a> 	static Monitor *mon = NULL;
<a href="#l1131" class="line" id="l1131">   1131</a> 	Monitor *m;
<a href="#l1132" class="line" id="l1132">   1132</a> 	XMotionEvent *ev = &amp;e-&gt;xmotion;
<a href="#l1133" class="line" id="l1133">   1133</a> 
<a href="#l1134" class="line" id="l1134">   1134</a> 	if (ev-&gt;window != root)
<a href="#l1135" class="line" id="l1135">   1135</a> 		return;
<a href="#l1136" class="line" id="l1136">   1136</a> 	if ((m = recttomon(ev-&gt;x_root, ev-&gt;y_root, 1, 1)) != mon &amp;&amp; mon) {
<a href="#l1137" class="line" id="l1137">   1137</a> 		unfocus(selmon-&gt;sel, 1);
<a href="#l1138" class="line" id="l1138">   1138</a> 		selmon = m;
<a href="#l1139" class="line" id="l1139">   1139</a> 		focus(NULL);
<a href="#l1140" class="line" id="l1140">   1140</a> 	}
<a href="#l1141" class="line" id="l1141">   1141</a> 	mon = m;
<a href="#l1142" class="line" id="l1142">   1142</a> }
<a href="#l1143" class="line" id="l1143">   1143</a> 
<a href="#l1144" class="line" id="l1144">   1144</a> void
<a href="#l1145" class="line" id="l1145">   1145</a> movemouse(const Arg *arg)
<a href="#l1146" class="line" id="l1146">   1146</a> {
<a href="#l1147" class="line" id="l1147">   1147</a> 	int x, y, ocx, ocy, nx, ny;
<a href="#l1148" class="line" id="l1148">   1148</a> 	Client *c;
<a href="#l1149" class="line" id="l1149">   1149</a> 	Monitor *m;
<a href="#l1150" class="line" id="l1150">   1150</a> 	XEvent ev;
<a href="#l1151" class="line" id="l1151">   1151</a> 	Time lasttime = 0;
<a href="#l1152" class="line" id="l1152">   1152</a> 
<a href="#l1153" class="line" id="l1153">   1153</a> 	if (!(c = selmon-&gt;sel))
<a href="#l1154" class="line" id="l1154">   1154</a> 		return;
<a href="#l1155" class="line" id="l1155">   1155</a> 	if (c-&gt;isfullscreen) /* no support moving fullscreen windows by mouse */
<a href="#l1156" class="line" id="l1156">   1156</a> 		return;
<a href="#l1157" class="line" id="l1157">   1157</a> 	restack(selmon);
<a href="#l1158" class="line" id="l1158">   1158</a> 	ocx = c-&gt;x;
<a href="#l1159" class="line" id="l1159">   1159</a> 	ocy = c-&gt;y;
<a href="#l1160" class="line" id="l1160">   1160</a> 	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
<a href="#l1161" class="line" id="l1161">   1161</a> 		None, cursor[CurMove]-&gt;cursor, CurrentTime) != GrabSuccess)
<a href="#l1162" class="line" id="l1162">   1162</a> 		return;
<a href="#l1163" class="line" id="l1163">   1163</a> 	if (!getrootptr(&amp;x, &amp;y))
<a href="#l1164" class="line" id="l1164">   1164</a> 		return;
<a href="#l1165" class="line" id="l1165">   1165</a> 	do {
<a href="#l1166" class="line" id="l1166">   1166</a> 		XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &amp;ev);
<a href="#l1167" class="line" id="l1167">   1167</a> 		switch(ev.type) {
<a href="#l1168" class="line" id="l1168">   1168</a> 		case ConfigureRequest:
<a href="#l1169" class="line" id="l1169">   1169</a> 		case Expose:
<a href="#l1170" class="line" id="l1170">   1170</a> 		case MapRequest:
<a href="#l1171" class="line" id="l1171">   1171</a> 			handler[ev.type](&amp;ev);
<a href="#l1172" class="line" id="l1172">   1172</a> 			break;
<a href="#l1173" class="line" id="l1173">   1173</a> 		case MotionNotify:
<a href="#l1174" class="line" id="l1174">   1174</a> 			if ((ev.xmotion.time - lasttime) &lt;= (1000 / 60))
<a href="#l1175" class="line" id="l1175">   1175</a> 				continue;
<a href="#l1176" class="line" id="l1176">   1176</a> 			lasttime = ev.xmotion.time;
<a href="#l1177" class="line" id="l1177">   1177</a> 
<a href="#l1178" class="line" id="l1178">   1178</a> 			nx = ocx + (ev.xmotion.x - x);
<a href="#l1179" class="line" id="l1179">   1179</a> 			ny = ocy + (ev.xmotion.y - y);
<a href="#l1180" class="line" id="l1180">   1180</a> 			if (abs(selmon-&gt;wx - nx) &lt; snap)
<a href="#l1181" class="line" id="l1181">   1181</a> 				nx = selmon-&gt;wx;
<a href="#l1182" class="line" id="l1182">   1182</a> 			else if (abs((selmon-&gt;wx + selmon-&gt;ww) - (nx + WIDTH(c))) &lt; snap)
<a href="#l1183" class="line" id="l1183">   1183</a> 				nx = selmon-&gt;wx + selmon-&gt;ww - WIDTH(c);
<a href="#l1184" class="line" id="l1184">   1184</a> 			if (abs(selmon-&gt;wy - ny) &lt; snap)
<a href="#l1185" class="line" id="l1185">   1185</a> 				ny = selmon-&gt;wy;
<a href="#l1186" class="line" id="l1186">   1186</a> 			else if (abs((selmon-&gt;wy + selmon-&gt;wh) - (ny + HEIGHT(c))) &lt; snap)
<a href="#l1187" class="line" id="l1187">   1187</a> 				ny = selmon-&gt;wy + selmon-&gt;wh - HEIGHT(c);
<a href="#l1188" class="line" id="l1188">   1188</a> 			if (!c-&gt;isfloating &amp;&amp; selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange
<a href="#l1189" class="line" id="l1189">   1189</a> 			&amp;&amp; (abs(nx - c-&gt;x) &gt; snap || abs(ny - c-&gt;y) &gt; snap))
<a href="#l1190" class="line" id="l1190">   1190</a> 				togglefloating(NULL);
<a href="#l1191" class="line" id="l1191">   1191</a> 			if (!selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange || c-&gt;isfloating)
<a href="#l1192" class="line" id="l1192">   1192</a> 				resize(c, nx, ny, c-&gt;w, c-&gt;h, 1);
<a href="#l1193" class="line" id="l1193">   1193</a> 			break;
<a href="#l1194" class="line" id="l1194">   1194</a> 		}
<a href="#l1195" class="line" id="l1195">   1195</a> 	} while (ev.type != ButtonRelease);
<a href="#l1196" class="line" id="l1196">   1196</a> 	XUngrabPointer(dpy, CurrentTime);
<a href="#l1197" class="line" id="l1197">   1197</a> 	if ((m = recttomon(c-&gt;x, c-&gt;y, c-&gt;w, c-&gt;h)) != selmon) {
<a href="#l1198" class="line" id="l1198">   1198</a> 		sendmon(c, m);
<a href="#l1199" class="line" id="l1199">   1199</a> 		selmon = m;
<a href="#l1200" class="line" id="l1200">   1200</a> 		focus(NULL);
<a href="#l1201" class="line" id="l1201">   1201</a> 	}
<a href="#l1202" class="line" id="l1202">   1202</a> }
<a href="#l1203" class="line" id="l1203">   1203</a> 
<a href="#l1204" class="line" id="l1204">   1204</a> Client *
<a href="#l1205" class="line" id="l1205">   1205</a> nexttiled(Client *c)
<a href="#l1206" class="line" id="l1206">   1206</a> {
<a href="#l1207" class="line" id="l1207">   1207</a> 	for (; c &amp;&amp; (c-&gt;isfloating || !ISVISIBLE(c)); c = c-&gt;next);
<a href="#l1208" class="line" id="l1208">   1208</a> 	return c;
<a href="#l1209" class="line" id="l1209">   1209</a> }
<a href="#l1210" class="line" id="l1210">   1210</a> 
<a href="#l1211" class="line" id="l1211">   1211</a> void
<a href="#l1212" class="line" id="l1212">   1212</a> pop(Client *c)
<a href="#l1213" class="line" id="l1213">   1213</a> {
<a href="#l1214" class="line" id="l1214">   1214</a> 	detach(c);
<a href="#l1215" class="line" id="l1215">   1215</a> 	attach(c);
<a href="#l1216" class="line" id="l1216">   1216</a> 	focus(c);
<a href="#l1217" class="line" id="l1217">   1217</a> 	arrange(c-&gt;mon);
<a href="#l1218" class="line" id="l1218">   1218</a> }
<a href="#l1219" class="line" id="l1219">   1219</a> 
<a href="#l1220" class="line" id="l1220">   1220</a> void
<a href="#l1221" class="line" id="l1221">   1221</a> propertynotify(XEvent *e)
<a href="#l1222" class="line" id="l1222">   1222</a> {
<a href="#l1223" class="line" id="l1223">   1223</a> 	Client *c;
<a href="#l1224" class="line" id="l1224">   1224</a> 	Window trans;
<a href="#l1225" class="line" id="l1225">   1225</a> 	XPropertyEvent *ev = &amp;e-&gt;xproperty;
<a href="#l1226" class="line" id="l1226">   1226</a> 
<a href="#l1227" class="line" id="l1227">   1227</a> 	if ((ev-&gt;window == root) &amp;&amp; (ev-&gt;atom == XA_WM_NAME))
<a href="#l1228" class="line" id="l1228">   1228</a> 		updatestatus();
<a href="#l1229" class="line" id="l1229">   1229</a> 	else if (ev-&gt;state == PropertyDelete)
<a href="#l1230" class="line" id="l1230">   1230</a> 		return; /* ignore */
<a href="#l1231" class="line" id="l1231">   1231</a> 	else if ((c = wintoclient(ev-&gt;window))) {
<a href="#l1232" class="line" id="l1232">   1232</a> 		switch(ev-&gt;atom) {
<a href="#l1233" class="line" id="l1233">   1233</a> 		default: break;
<a href="#l1234" class="line" id="l1234">   1234</a> 		case XA_WM_TRANSIENT_FOR:
<a href="#l1235" class="line" id="l1235">   1235</a> 			if (!c-&gt;isfloating &amp;&amp; (XGetTransientForHint(dpy, c-&gt;win, &amp;trans)) &amp;&amp;
<a href="#l1236" class="line" id="l1236">   1236</a> 				(c-&gt;isfloating = (wintoclient(trans)) != NULL))
<a href="#l1237" class="line" id="l1237">   1237</a> 				arrange(c-&gt;mon);
<a href="#l1238" class="line" id="l1238">   1238</a> 			break;
<a href="#l1239" class="line" id="l1239">   1239</a> 		case XA_WM_NORMAL_HINTS:
<a href="#l1240" class="line" id="l1240">   1240</a> 			c-&gt;hintsvalid = 0;
<a href="#l1241" class="line" id="l1241">   1241</a> 			break;
<a href="#l1242" class="line" id="l1242">   1242</a> 		case XA_WM_HINTS:
<a href="#l1243" class="line" id="l1243">   1243</a> 			updatewmhints(c);
<a href="#l1244" class="line" id="l1244">   1244</a> 			drawbars();
<a href="#l1245" class="line" id="l1245">   1245</a> 			break;
<a href="#l1246" class="line" id="l1246">   1246</a> 		}
<a href="#l1247" class="line" id="l1247">   1247</a> 		if (ev-&gt;atom == XA_WM_NAME || ev-&gt;atom == netatom[NetWMName]) {
<a href="#l1248" class="line" id="l1248">   1248</a> 			updatetitle(c);
<a href="#l1249" class="line" id="l1249">   1249</a> 			if (c == c-&gt;mon-&gt;sel)
<a href="#l1250" class="line" id="l1250">   1250</a> 				drawbar(c-&gt;mon);
<a href="#l1251" class="line" id="l1251">   1251</a> 		}
<a href="#l1252" class="line" id="l1252">   1252</a> 		if (ev-&gt;atom == netatom[NetWMWindowType])
<a href="#l1253" class="line" id="l1253">   1253</a> 			updatewindowtype(c);
<a href="#l1254" class="line" id="l1254">   1254</a> 	}
<a href="#l1255" class="line" id="l1255">   1255</a> }
<a href="#l1256" class="line" id="l1256">   1256</a> 
<a href="#l1257" class="line" id="l1257">   1257</a> void
<a href="#l1258" class="line" id="l1258">   1258</a> quit(const Arg *arg)
<a href="#l1259" class="line" id="l1259">   1259</a> {
<a href="#l1260" class="line" id="l1260">   1260</a> 	running = 0;
<a href="#l1261" class="line" id="l1261">   1261</a> }
<a href="#l1262" class="line" id="l1262">   1262</a> 
<a href="#l1263" class="line" id="l1263">   1263</a> Monitor *
<a href="#l1264" class="line" id="l1264">   1264</a> recttomon(int x, int y, int w, int h)
<a href="#l1265" class="line" id="l1265">   1265</a> {
<a href="#l1266" class="line" id="l1266">   1266</a> 	Monitor *m, *r = selmon;
<a href="#l1267" class="line" id="l1267">   1267</a> 	int a, area = 0;
<a href="#l1268" class="line" id="l1268">   1268</a> 
<a href="#l1269" class="line" id="l1269">   1269</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l1270" class="line" id="l1270">   1270</a> 		if ((a = INTERSECT(x, y, w, h, m)) &gt; area) {
<a href="#l1271" class="line" id="l1271">   1271</a> 			area = a;
<a href="#l1272" class="line" id="l1272">   1272</a> 			r = m;
<a href="#l1273" class="line" id="l1273">   1273</a> 		}
<a href="#l1274" class="line" id="l1274">   1274</a> 	return r;
<a href="#l1275" class="line" id="l1275">   1275</a> }
<a href="#l1276" class="line" id="l1276">   1276</a> 
<a href="#l1277" class="line" id="l1277">   1277</a> void
<a href="#l1278" class="line" id="l1278">   1278</a> resize(Client *c, int x, int y, int w, int h, int interact)
<a href="#l1279" class="line" id="l1279">   1279</a> {
<a href="#l1280" class="line" id="l1280">   1280</a> 	if (applysizehints(c, &amp;x, &amp;y, &amp;w, &amp;h, interact))
<a href="#l1281" class="line" id="l1281">   1281</a> 		resizeclient(c, x, y, w, h);
<a href="#l1282" class="line" id="l1282">   1282</a> }
<a href="#l1283" class="line" id="l1283">   1283</a> 
<a href="#l1284" class="line" id="l1284">   1284</a> void
<a href="#l1285" class="line" id="l1285">   1285</a> resizeclient(Client *c, int x, int y, int w, int h)
<a href="#l1286" class="line" id="l1286">   1286</a> {
<a href="#l1287" class="line" id="l1287">   1287</a> 	XWindowChanges wc;
<a href="#l1288" class="line" id="l1288">   1288</a> 
<a href="#l1289" class="line" id="l1289">   1289</a> 	c-&gt;oldx = c-&gt;x; c-&gt;x = wc.x = x;
<a href="#l1290" class="line" id="l1290">   1290</a> 	c-&gt;oldy = c-&gt;y; c-&gt;y = wc.y = y;
<a href="#l1291" class="line" id="l1291">   1291</a> 	c-&gt;oldw = c-&gt;w; c-&gt;w = wc.width = w;
<a href="#l1292" class="line" id="l1292">   1292</a> 	c-&gt;oldh = c-&gt;h; c-&gt;h = wc.height = h;
<a href="#l1293" class="line" id="l1293">   1293</a> 	wc.border_width = c-&gt;bw;
<a href="#l1294" class="line" id="l1294">   1294</a> 	XConfigureWindow(dpy, c-&gt;win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &amp;wc);
<a href="#l1295" class="line" id="l1295">   1295</a> 	configure(c);
<a href="#l1296" class="line" id="l1296">   1296</a> 	XSync(dpy, False);
<a href="#l1297" class="line" id="l1297">   1297</a> }
<a href="#l1298" class="line" id="l1298">   1298</a> 
<a href="#l1299" class="line" id="l1299">   1299</a> void
<a href="#l1300" class="line" id="l1300">   1300</a> resizemouse(const Arg *arg)
<a href="#l1301" class="line" id="l1301">   1301</a> {
<a href="#l1302" class="line" id="l1302">   1302</a> 	int ocx, ocy, nw, nh;
<a href="#l1303" class="line" id="l1303">   1303</a> 	Client *c;
<a href="#l1304" class="line" id="l1304">   1304</a> 	Monitor *m;
<a href="#l1305" class="line" id="l1305">   1305</a> 	XEvent ev;
<a href="#l1306" class="line" id="l1306">   1306</a> 	Time lasttime = 0;
<a href="#l1307" class="line" id="l1307">   1307</a> 
<a href="#l1308" class="line" id="l1308">   1308</a> 	if (!(c = selmon-&gt;sel))
<a href="#l1309" class="line" id="l1309">   1309</a> 		return;
<a href="#l1310" class="line" id="l1310">   1310</a> 	if (c-&gt;isfullscreen) /* no support resizing fullscreen windows by mouse */
<a href="#l1311" class="line" id="l1311">   1311</a> 		return;
<a href="#l1312" class="line" id="l1312">   1312</a> 	restack(selmon);
<a href="#l1313" class="line" id="l1313">   1313</a> 	ocx = c-&gt;x;
<a href="#l1314" class="line" id="l1314">   1314</a> 	ocy = c-&gt;y;
<a href="#l1315" class="line" id="l1315">   1315</a> 	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
<a href="#l1316" class="line" id="l1316">   1316</a> 		None, cursor[CurResize]-&gt;cursor, CurrentTime) != GrabSuccess)
<a href="#l1317" class="line" id="l1317">   1317</a> 		return;
<a href="#l1318" class="line" id="l1318">   1318</a> 	XWarpPointer(dpy, None, c-&gt;win, 0, 0, 0, 0, c-&gt;w + c-&gt;bw - 1, c-&gt;h + c-&gt;bw - 1);
<a href="#l1319" class="line" id="l1319">   1319</a> 	do {
<a href="#l1320" class="line" id="l1320">   1320</a> 		XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &amp;ev);
<a href="#l1321" class="line" id="l1321">   1321</a> 		switch(ev.type) {
<a href="#l1322" class="line" id="l1322">   1322</a> 		case ConfigureRequest:
<a href="#l1323" class="line" id="l1323">   1323</a> 		case Expose:
<a href="#l1324" class="line" id="l1324">   1324</a> 		case MapRequest:
<a href="#l1325" class="line" id="l1325">   1325</a> 			handler[ev.type](&amp;ev);
<a href="#l1326" class="line" id="l1326">   1326</a> 			break;
<a href="#l1327" class="line" id="l1327">   1327</a> 		case MotionNotify:
<a href="#l1328" class="line" id="l1328">   1328</a> 			if ((ev.xmotion.time - lasttime) &lt;= (1000 / 60))
<a href="#l1329" class="line" id="l1329">   1329</a> 				continue;
<a href="#l1330" class="line" id="l1330">   1330</a> 			lasttime = ev.xmotion.time;
<a href="#l1331" class="line" id="l1331">   1331</a> 
<a href="#l1332" class="line" id="l1332">   1332</a> 			nw = MAX(ev.xmotion.x - ocx - 2 * c-&gt;bw + 1, 1);
<a href="#l1333" class="line" id="l1333">   1333</a> 			nh = MAX(ev.xmotion.y - ocy - 2 * c-&gt;bw + 1, 1);
<a href="#l1334" class="line" id="l1334">   1334</a> 			if (c-&gt;mon-&gt;wx + nw &gt;= selmon-&gt;wx &amp;&amp; c-&gt;mon-&gt;wx + nw &lt;= selmon-&gt;wx + selmon-&gt;ww
<a href="#l1335" class="line" id="l1335">   1335</a> 			&amp;&amp; c-&gt;mon-&gt;wy + nh &gt;= selmon-&gt;wy &amp;&amp; c-&gt;mon-&gt;wy + nh &lt;= selmon-&gt;wy + selmon-&gt;wh)
<a href="#l1336" class="line" id="l1336">   1336</a> 			{
<a href="#l1337" class="line" id="l1337">   1337</a> 				if (!c-&gt;isfloating &amp;&amp; selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange
<a href="#l1338" class="line" id="l1338">   1338</a> 				&amp;&amp; (abs(nw - c-&gt;w) &gt; snap || abs(nh - c-&gt;h) &gt; snap))
<a href="#l1339" class="line" id="l1339">   1339</a> 					togglefloating(NULL);
<a href="#l1340" class="line" id="l1340">   1340</a> 			}
<a href="#l1341" class="line" id="l1341">   1341</a> 			if (!selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange || c-&gt;isfloating)
<a href="#l1342" class="line" id="l1342">   1342</a> 				resize(c, c-&gt;x, c-&gt;y, nw, nh, 1);
<a href="#l1343" class="line" id="l1343">   1343</a> 			break;
<a href="#l1344" class="line" id="l1344">   1344</a> 		}
<a href="#l1345" class="line" id="l1345">   1345</a> 	} while (ev.type != ButtonRelease);
<a href="#l1346" class="line" id="l1346">   1346</a> 	XWarpPointer(dpy, None, c-&gt;win, 0, 0, 0, 0, c-&gt;w + c-&gt;bw - 1, c-&gt;h + c-&gt;bw - 1);
<a href="#l1347" class="line" id="l1347">   1347</a> 	XUngrabPointer(dpy, CurrentTime);
<a href="#l1348" class="line" id="l1348">   1348</a> 	while (XCheckMaskEvent(dpy, EnterWindowMask, &amp;ev));
<a href="#l1349" class="line" id="l1349">   1349</a> 	if ((m = recttomon(c-&gt;x, c-&gt;y, c-&gt;w, c-&gt;h)) != selmon) {
<a href="#l1350" class="line" id="l1350">   1350</a> 		sendmon(c, m);
<a href="#l1351" class="line" id="l1351">   1351</a> 		selmon = m;
<a href="#l1352" class="line" id="l1352">   1352</a> 		focus(NULL);
<a href="#l1353" class="line" id="l1353">   1353</a> 	}
<a href="#l1354" class="line" id="l1354">   1354</a> }
<a href="#l1355" class="line" id="l1355">   1355</a> 
<a href="#l1356" class="line" id="l1356">   1356</a> void
<a href="#l1357" class="line" id="l1357">   1357</a> restack(Monitor *m)
<a href="#l1358" class="line" id="l1358">   1358</a> {
<a href="#l1359" class="line" id="l1359">   1359</a> 	Client *c;
<a href="#l1360" class="line" id="l1360">   1360</a> 	XEvent ev;
<a href="#l1361" class="line" id="l1361">   1361</a> 	XWindowChanges wc;
<a href="#l1362" class="line" id="l1362">   1362</a> 
<a href="#l1363" class="line" id="l1363">   1363</a> 	drawbar(m);
<a href="#l1364" class="line" id="l1364">   1364</a> 	if (!m-&gt;sel)
<a href="#l1365" class="line" id="l1365">   1365</a> 		return;
<a href="#l1366" class="line" id="l1366">   1366</a> 	if (m-&gt;sel-&gt;isfloating || !m-&gt;lt[m-&gt;sellt]-&gt;arrange)
<a href="#l1367" class="line" id="l1367">   1367</a> 		XRaiseWindow(dpy, m-&gt;sel-&gt;win);
<a href="#l1368" class="line" id="l1368">   1368</a> 	if (m-&gt;lt[m-&gt;sellt]-&gt;arrange) {
<a href="#l1369" class="line" id="l1369">   1369</a> 		wc.stack_mode = Below;
<a href="#l1370" class="line" id="l1370">   1370</a> 		wc.sibling = m-&gt;barwin;
<a href="#l1371" class="line" id="l1371">   1371</a> 		for (c = m-&gt;stack; c; c = c-&gt;snext)
<a href="#l1372" class="line" id="l1372">   1372</a> 			if (!c-&gt;isfloating &amp;&amp; ISVISIBLE(c)) {
<a href="#l1373" class="line" id="l1373">   1373</a> 				XConfigureWindow(dpy, c-&gt;win, CWSibling|CWStackMode, &amp;wc);
<a href="#l1374" class="line" id="l1374">   1374</a> 				wc.sibling = c-&gt;win;
<a href="#l1375" class="line" id="l1375">   1375</a> 			}
<a href="#l1376" class="line" id="l1376">   1376</a> 	}
<a href="#l1377" class="line" id="l1377">   1377</a> 	XSync(dpy, False);
<a href="#l1378" class="line" id="l1378">   1378</a> 	while (XCheckMaskEvent(dpy, EnterWindowMask, &amp;ev));
<a href="#l1379" class="line" id="l1379">   1379</a> }
<a href="#l1380" class="line" id="l1380">   1380</a> 
<a href="#l1381" class="line" id="l1381">   1381</a> void
<a href="#l1382" class="line" id="l1382">   1382</a> run(void)
<a href="#l1383" class="line" id="l1383">   1383</a> {
<a href="#l1384" class="line" id="l1384">   1384</a> 	XEvent ev;
<a href="#l1385" class="line" id="l1385">   1385</a> 	/* main event loop */
<a href="#l1386" class="line" id="l1386">   1386</a> 	XSync(dpy, False);
<a href="#l1387" class="line" id="l1387">   1387</a> 	while (running &amp;&amp; !XNextEvent(dpy, &amp;ev))
<a href="#l1388" class="line" id="l1388">   1388</a> 		if (handler[ev.type])
<a href="#l1389" class="line" id="l1389">   1389</a> 			handler[ev.type](&amp;ev); /* call handler */
<a href="#l1390" class="line" id="l1390">   1390</a> }
<a href="#l1391" class="line" id="l1391">   1391</a> 
<a href="#l1392" class="line" id="l1392">   1392</a> void
<a href="#l1393" class="line" id="l1393">   1393</a> scan(void)
<a href="#l1394" class="line" id="l1394">   1394</a> {
<a href="#l1395" class="line" id="l1395">   1395</a> 	unsigned int i, num;
<a href="#l1396" class="line" id="l1396">   1396</a> 	Window d1, d2, *wins = NULL;
<a href="#l1397" class="line" id="l1397">   1397</a> 	XWindowAttributes wa;
<a href="#l1398" class="line" id="l1398">   1398</a> 
<a href="#l1399" class="line" id="l1399">   1399</a> 	if (XQueryTree(dpy, root, &amp;d1, &amp;d2, &amp;wins, &amp;num)) {
<a href="#l1400" class="line" id="l1400">   1400</a> 		for (i = 0; i &lt; num; i++) {
<a href="#l1401" class="line" id="l1401">   1401</a> 			if (!XGetWindowAttributes(dpy, wins[i], &amp;wa)
<a href="#l1402" class="line" id="l1402">   1402</a> 			|| wa.override_redirect || XGetTransientForHint(dpy, wins[i], &amp;d1))
<a href="#l1403" class="line" id="l1403">   1403</a> 				continue;
<a href="#l1404" class="line" id="l1404">   1404</a> 			if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
<a href="#l1405" class="line" id="l1405">   1405</a> 				manage(wins[i], &amp;wa);
<a href="#l1406" class="line" id="l1406">   1406</a> 		}
<a href="#l1407" class="line" id="l1407">   1407</a> 		for (i = 0; i &lt; num; i++) { /* now the transients */
<a href="#l1408" class="line" id="l1408">   1408</a> 			if (!XGetWindowAttributes(dpy, wins[i], &amp;wa))
<a href="#l1409" class="line" id="l1409">   1409</a> 				continue;
<a href="#l1410" class="line" id="l1410">   1410</a> 			if (XGetTransientForHint(dpy, wins[i], &amp;d1)
<a href="#l1411" class="line" id="l1411">   1411</a> 			&amp;&amp; (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
<a href="#l1412" class="line" id="l1412">   1412</a> 				manage(wins[i], &amp;wa);
<a href="#l1413" class="line" id="l1413">   1413</a> 		}
<a href="#l1414" class="line" id="l1414">   1414</a> 		if (wins)
<a href="#l1415" class="line" id="l1415">   1415</a> 			XFree(wins);
<a href="#l1416" class="line" id="l1416">   1416</a> 	}
<a href="#l1417" class="line" id="l1417">   1417</a> }
<a href="#l1418" class="line" id="l1418">   1418</a> 
<a href="#l1419" class="line" id="l1419">   1419</a> void
<a href="#l1420" class="line" id="l1420">   1420</a> sendmon(Client *c, Monitor *m)
<a href="#l1421" class="line" id="l1421">   1421</a> {
<a href="#l1422" class="line" id="l1422">   1422</a> 	if (c-&gt;mon == m)
<a href="#l1423" class="line" id="l1423">   1423</a> 		return;
<a href="#l1424" class="line" id="l1424">   1424</a> 	unfocus(c, 1);
<a href="#l1425" class="line" id="l1425">   1425</a> 	detach(c);
<a href="#l1426" class="line" id="l1426">   1426</a> 	detachstack(c);
<a href="#l1427" class="line" id="l1427">   1427</a> 	c-&gt;mon = m;
<a href="#l1428" class="line" id="l1428">   1428</a> 	c-&gt;tags = m-&gt;tagset[m-&gt;seltags]; /* assign tags of target monitor */
<a href="#l1429" class="line" id="l1429">   1429</a> 	attach(c);
<a href="#l1430" class="line" id="l1430">   1430</a> 	attachstack(c);
<a href="#l1431" class="line" id="l1431">   1431</a> 	focus(NULL);
<a href="#l1432" class="line" id="l1432">   1432</a> 	arrange(NULL);
<a href="#l1433" class="line" id="l1433">   1433</a> }
<a href="#l1434" class="line" id="l1434">   1434</a> 
<a href="#l1435" class="line" id="l1435">   1435</a> void
<a href="#l1436" class="line" id="l1436">   1436</a> setclientstate(Client *c, long state)
<a href="#l1437" class="line" id="l1437">   1437</a> {
<a href="#l1438" class="line" id="l1438">   1438</a> 	long data[] = { state, None };
<a href="#l1439" class="line" id="l1439">   1439</a> 
<a href="#l1440" class="line" id="l1440">   1440</a> 	XChangeProperty(dpy, c-&gt;win, wmatom[WMState], wmatom[WMState], 32,
<a href="#l1441" class="line" id="l1441">   1441</a> 		PropModeReplace, (unsigned char *)data, 2);
<a href="#l1442" class="line" id="l1442">   1442</a> }
<a href="#l1443" class="line" id="l1443">   1443</a> 
<a href="#l1444" class="line" id="l1444">   1444</a> int
<a href="#l1445" class="line" id="l1445">   1445</a> sendevent(Client *c, Atom proto)
<a href="#l1446" class="line" id="l1446">   1446</a> {
<a href="#l1447" class="line" id="l1447">   1447</a> 	int n;
<a href="#l1448" class="line" id="l1448">   1448</a> 	Atom *protocols;
<a href="#l1449" class="line" id="l1449">   1449</a> 	int exists = 0;
<a href="#l1450" class="line" id="l1450">   1450</a> 	XEvent ev;
<a href="#l1451" class="line" id="l1451">   1451</a> 
<a href="#l1452" class="line" id="l1452">   1452</a> 	if (XGetWMProtocols(dpy, c-&gt;win, &amp;protocols, &amp;n)) {
<a href="#l1453" class="line" id="l1453">   1453</a> 		while (!exists &amp;&amp; n--)
<a href="#l1454" class="line" id="l1454">   1454</a> 			exists = protocols[n] == proto;
<a href="#l1455" class="line" id="l1455">   1455</a> 		XFree(protocols);
<a href="#l1456" class="line" id="l1456">   1456</a> 	}
<a href="#l1457" class="line" id="l1457">   1457</a> 	if (exists) {
<a href="#l1458" class="line" id="l1458">   1458</a> 		ev.type = ClientMessage;
<a href="#l1459" class="line" id="l1459">   1459</a> 		ev.xclient.window = c-&gt;win;
<a href="#l1460" class="line" id="l1460">   1460</a> 		ev.xclient.message_type = wmatom[WMProtocols];
<a href="#l1461" class="line" id="l1461">   1461</a> 		ev.xclient.format = 32;
<a href="#l1462" class="line" id="l1462">   1462</a> 		ev.xclient.data.l[0] = proto;
<a href="#l1463" class="line" id="l1463">   1463</a> 		ev.xclient.data.l[1] = CurrentTime;
<a href="#l1464" class="line" id="l1464">   1464</a> 		XSendEvent(dpy, c-&gt;win, False, NoEventMask, &amp;ev);
<a href="#l1465" class="line" id="l1465">   1465</a> 	}
<a href="#l1466" class="line" id="l1466">   1466</a> 	return exists;
<a href="#l1467" class="line" id="l1467">   1467</a> }
<a href="#l1468" class="line" id="l1468">   1468</a> 
<a href="#l1469" class="line" id="l1469">   1469</a> void
<a href="#l1470" class="line" id="l1470">   1470</a> setfocus(Client *c)
<a href="#l1471" class="line" id="l1471">   1471</a> {
<a href="#l1472" class="line" id="l1472">   1472</a> 	if (!c-&gt;neverfocus) {
<a href="#l1473" class="line" id="l1473">   1473</a> 		XSetInputFocus(dpy, c-&gt;win, RevertToPointerRoot, CurrentTime);
<a href="#l1474" class="line" id="l1474">   1474</a> 		XChangeProperty(dpy, root, netatom[NetActiveWindow],
<a href="#l1475" class="line" id="l1475">   1475</a> 			XA_WINDOW, 32, PropModeReplace,
<a href="#l1476" class="line" id="l1476">   1476</a> 			(unsigned char *) &amp;(c-&gt;win), 1);
<a href="#l1477" class="line" id="l1477">   1477</a> 	}
<a href="#l1478" class="line" id="l1478">   1478</a> 	sendevent(c, wmatom[WMTakeFocus]);
<a href="#l1479" class="line" id="l1479">   1479</a> }
<a href="#l1480" class="line" id="l1480">   1480</a> 
<a href="#l1481" class="line" id="l1481">   1481</a> void
<a href="#l1482" class="line" id="l1482">   1482</a> setfullscreen(Client *c, int fullscreen)
<a href="#l1483" class="line" id="l1483">   1483</a> {
<a href="#l1484" class="line" id="l1484">   1484</a> 	if (fullscreen &amp;&amp; !c-&gt;isfullscreen) {
<a href="#l1485" class="line" id="l1485">   1485</a> 		XChangeProperty(dpy, c-&gt;win, netatom[NetWMState], XA_ATOM, 32,
<a href="#l1486" class="line" id="l1486">   1486</a> 			PropModeReplace, (unsigned char*)&amp;netatom[NetWMFullscreen], 1);
<a href="#l1487" class="line" id="l1487">   1487</a> 		c-&gt;isfullscreen = 1;
<a href="#l1488" class="line" id="l1488">   1488</a> 		c-&gt;oldstate = c-&gt;isfloating;
<a href="#l1489" class="line" id="l1489">   1489</a> 		c-&gt;oldbw = c-&gt;bw;
<a href="#l1490" class="line" id="l1490">   1490</a> 		c-&gt;bw = 0;
<a href="#l1491" class="line" id="l1491">   1491</a> 		c-&gt;isfloating = 1;
<a href="#l1492" class="line" id="l1492">   1492</a> 		resizeclient(c, c-&gt;mon-&gt;mx, c-&gt;mon-&gt;my, c-&gt;mon-&gt;mw, c-&gt;mon-&gt;mh);
<a href="#l1493" class="line" id="l1493">   1493</a> 		XRaiseWindow(dpy, c-&gt;win);
<a href="#l1494" class="line" id="l1494">   1494</a> 	} else if (!fullscreen &amp;&amp; c-&gt;isfullscreen){
<a href="#l1495" class="line" id="l1495">   1495</a> 		XChangeProperty(dpy, c-&gt;win, netatom[NetWMState], XA_ATOM, 32,
<a href="#l1496" class="line" id="l1496">   1496</a> 			PropModeReplace, (unsigned char*)0, 0);
<a href="#l1497" class="line" id="l1497">   1497</a> 		c-&gt;isfullscreen = 0;
<a href="#l1498" class="line" id="l1498">   1498</a> 		c-&gt;isfloating = c-&gt;oldstate;
<a href="#l1499" class="line" id="l1499">   1499</a> 		c-&gt;bw = c-&gt;oldbw;
<a href="#l1500" class="line" id="l1500">   1500</a> 		c-&gt;x = c-&gt;oldx;
<a href="#l1501" class="line" id="l1501">   1501</a> 		c-&gt;y = c-&gt;oldy;
<a href="#l1502" class="line" id="l1502">   1502</a> 		c-&gt;w = c-&gt;oldw;
<a href="#l1503" class="line" id="l1503">   1503</a> 		c-&gt;h = c-&gt;oldh;
<a href="#l1504" class="line" id="l1504">   1504</a> 		resizeclient(c, c-&gt;x, c-&gt;y, c-&gt;w, c-&gt;h);
<a href="#l1505" class="line" id="l1505">   1505</a> 		arrange(c-&gt;mon);
<a href="#l1506" class="line" id="l1506">   1506</a> 	}
<a href="#l1507" class="line" id="l1507">   1507</a> }
<a href="#l1508" class="line" id="l1508">   1508</a> 
<a href="#l1509" class="line" id="l1509">   1509</a> void
<a href="#l1510" class="line" id="l1510">   1510</a> setlayout(const Arg *arg)
<a href="#l1511" class="line" id="l1511">   1511</a> {
<a href="#l1512" class="line" id="l1512">   1512</a> 	if (!arg || !arg-&gt;v || arg-&gt;v != selmon-&gt;lt[selmon-&gt;sellt])
<a href="#l1513" class="line" id="l1513">   1513</a> 		selmon-&gt;sellt ^= 1;
<a href="#l1514" class="line" id="l1514">   1514</a> 	if (arg &amp;&amp; arg-&gt;v)
<a href="#l1515" class="line" id="l1515">   1515</a> 		selmon-&gt;lt[selmon-&gt;sellt] = (Layout *)arg-&gt;v;
<a href="#l1516" class="line" id="l1516">   1516</a> 	strncpy(selmon-&gt;ltsymbol, selmon-&gt;lt[selmon-&gt;sellt]-&gt;symbol, sizeof selmon-&gt;ltsymbol);
<a href="#l1517" class="line" id="l1517">   1517</a> 	if (selmon-&gt;sel)
<a href="#l1518" class="line" id="l1518">   1518</a> 		arrange(selmon);
<a href="#l1519" class="line" id="l1519">   1519</a> 	else
<a href="#l1520" class="line" id="l1520">   1520</a> 		drawbar(selmon);
<a href="#l1521" class="line" id="l1521">   1521</a> }
<a href="#l1522" class="line" id="l1522">   1522</a> 
<a href="#l1523" class="line" id="l1523">   1523</a> /* arg &gt; 1.0 will set mfact absolutely */
<a href="#l1524" class="line" id="l1524">   1524</a> void
<a href="#l1525" class="line" id="l1525">   1525</a> setmfact(const Arg *arg)
<a href="#l1526" class="line" id="l1526">   1526</a> {
<a href="#l1527" class="line" id="l1527">   1527</a> 	float f;
<a href="#l1528" class="line" id="l1528">   1528</a> 
<a href="#l1529" class="line" id="l1529">   1529</a> 	if (!arg || !selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange)
<a href="#l1530" class="line" id="l1530">   1530</a> 		return;
<a href="#l1531" class="line" id="l1531">   1531</a> 	f = arg-&gt;f &lt; 1.0 ? arg-&gt;f + selmon-&gt;mfact : arg-&gt;f - 1.0;
<a href="#l1532" class="line" id="l1532">   1532</a> 	if (f &lt; 0.05 || f &gt; 0.95)
<a href="#l1533" class="line" id="l1533">   1533</a> 		return;
<a href="#l1534" class="line" id="l1534">   1534</a> 	selmon-&gt;mfact = f;
<a href="#l1535" class="line" id="l1535">   1535</a> 	arrange(selmon);
<a href="#l1536" class="line" id="l1536">   1536</a> }
<a href="#l1537" class="line" id="l1537">   1537</a> 
<a href="#l1538" class="line" id="l1538">   1538</a> void
<a href="#l1539" class="line" id="l1539">   1539</a> setup(void)
<a href="#l1540" class="line" id="l1540">   1540</a> {
<a href="#l1541" class="line" id="l1541">   1541</a> 	int i;
<a href="#l1542" class="line" id="l1542">   1542</a> 	XSetWindowAttributes wa;
<a href="#l1543" class="line" id="l1543">   1543</a> 	Atom utf8string;
<a href="#l1544" class="line" id="l1544">   1544</a> 	struct sigaction sa;
<a href="#l1545" class="line" id="l1545">   1545</a> 
<a href="#l1546" class="line" id="l1546">   1546</a> 	/* do not transform children into zombies when they terminate */
<a href="#l1547" class="line" id="l1547">   1547</a> 	sigemptyset(&amp;sa.sa_mask);
<a href="#l1548" class="line" id="l1548">   1548</a> 	sa.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT | SA_RESTART;
<a href="#l1549" class="line" id="l1549">   1549</a> 	sa.sa_handler = SIG_IGN;
<a href="#l1550" class="line" id="l1550">   1550</a> 	sigaction(SIGCHLD, &amp;sa, NULL);
<a href="#l1551" class="line" id="l1551">   1551</a> 
<a href="#l1552" class="line" id="l1552">   1552</a> 	/* clean up any zombies (inherited from .xinitrc etc) immediately */
<a href="#l1553" class="line" id="l1553">   1553</a> 	while (waitpid(-1, NULL, WNOHANG) &gt; 0);
<a href="#l1554" class="line" id="l1554">   1554</a> 
<a href="#l1555" class="line" id="l1555">   1555</a> 	/* init screen */
<a href="#l1556" class="line" id="l1556">   1556</a> 	screen = DefaultScreen(dpy);
<a href="#l1557" class="line" id="l1557">   1557</a> 	sw = DisplayWidth(dpy, screen);
<a href="#l1558" class="line" id="l1558">   1558</a> 	sh = DisplayHeight(dpy, screen);
<a href="#l1559" class="line" id="l1559">   1559</a> 	root = RootWindow(dpy, screen);
<a href="#l1560" class="line" id="l1560">   1560</a> 	drw = drw_create(dpy, screen, root, sw, sh);
<a href="#l1561" class="line" id="l1561">   1561</a> 	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
<a href="#l1562" class="line" id="l1562">   1562</a> 		die(&quot;no fonts could be loaded.&quot;);
<a href="#l1563" class="line" id="l1563">   1563</a> 	lrpad = drw-&gt;fonts-&gt;h;
<a href="#l1564" class="line" id="l1564">   1564</a> 	bh = drw-&gt;fonts-&gt;h + 2;
<a href="#l1565" class="line" id="l1565">   1565</a> 	updategeom();
<a href="#l1566" class="line" id="l1566">   1566</a> 	/* init atoms */
<a href="#l1567" class="line" id="l1567">   1567</a> 	utf8string = XInternAtom(dpy, &quot;UTF8_STRING&quot;, False);
<a href="#l1568" class="line" id="l1568">   1568</a> 	wmatom[WMProtocols] = XInternAtom(dpy, &quot;WM_PROTOCOLS&quot;, False);
<a href="#l1569" class="line" id="l1569">   1569</a> 	wmatom[WMDelete] = XInternAtom(dpy, &quot;WM_DELETE_WINDOW&quot;, False);
<a href="#l1570" class="line" id="l1570">   1570</a> 	wmatom[WMState] = XInternAtom(dpy, &quot;WM_STATE&quot;, False);
<a href="#l1571" class="line" id="l1571">   1571</a> 	wmatom[WMTakeFocus] = XInternAtom(dpy, &quot;WM_TAKE_FOCUS&quot;, False);
<a href="#l1572" class="line" id="l1572">   1572</a> 	netatom[NetActiveWindow] = XInternAtom(dpy, &quot;_NET_ACTIVE_WINDOW&quot;, False);
<a href="#l1573" class="line" id="l1573">   1573</a> 	netatom[NetSupported] = XInternAtom(dpy, &quot;_NET_SUPPORTED&quot;, False);
<a href="#l1574" class="line" id="l1574">   1574</a> 	netatom[NetWMName] = XInternAtom(dpy, &quot;_NET_WM_NAME&quot;, False);
<a href="#l1575" class="line" id="l1575">   1575</a> 	netatom[NetWMState] = XInternAtom(dpy, &quot;_NET_WM_STATE&quot;, False);
<a href="#l1576" class="line" id="l1576">   1576</a> 	netatom[NetWMCheck] = XInternAtom(dpy, &quot;_NET_SUPPORTING_WM_CHECK&quot;, False);
<a href="#l1577" class="line" id="l1577">   1577</a> 	netatom[NetWMFullscreen] = XInternAtom(dpy, &quot;_NET_WM_STATE_FULLSCREEN&quot;, False);
<a href="#l1578" class="line" id="l1578">   1578</a> 	netatom[NetWMWindowType] = XInternAtom(dpy, &quot;_NET_WM_WINDOW_TYPE&quot;, False);
<a href="#l1579" class="line" id="l1579">   1579</a> 	netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, &quot;_NET_WM_WINDOW_TYPE_DIALOG&quot;, False);
<a href="#l1580" class="line" id="l1580">   1580</a> 	netatom[NetClientList] = XInternAtom(dpy, &quot;_NET_CLIENT_LIST&quot;, False);
<a href="#l1581" class="line" id="l1581">   1581</a> 	/* init cursors */
<a href="#l1582" class="line" id="l1582">   1582</a> 	cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
<a href="#l1583" class="line" id="l1583">   1583</a> 	cursor[CurResize] = drw_cur_create(drw, XC_sizing);
<a href="#l1584" class="line" id="l1584">   1584</a> 	cursor[CurMove] = drw_cur_create(drw, XC_fleur);
<a href="#l1585" class="line" id="l1585">   1585</a> 	/* init appearance */
<a href="#l1586" class="line" id="l1586">   1586</a> 	scheme = ecalloc(LENGTH(colors), sizeof(Clr *));
<a href="#l1587" class="line" id="l1587">   1587</a> 	for (i = 0; i &lt; LENGTH(colors); i++)
<a href="#l1588" class="line" id="l1588">   1588</a> 		scheme[i] = drw_scm_create(drw, colors[i], 3);
<a href="#l1589" class="line" id="l1589">   1589</a> 	/* init bars */
<a href="#l1590" class="line" id="l1590">   1590</a> 	updatebars();
<a href="#l1591" class="line" id="l1591">   1591</a> 	updatestatus();
<a href="#l1592" class="line" id="l1592">   1592</a> 	/* supporting window for NetWMCheck */
<a href="#l1593" class="line" id="l1593">   1593</a> 	wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
<a href="#l1594" class="line" id="l1594">   1594</a> 	XChangeProperty(dpy, wmcheckwin, netatom[NetWMCheck], XA_WINDOW, 32,
<a href="#l1595" class="line" id="l1595">   1595</a> 		PropModeReplace, (unsigned char *) &amp;wmcheckwin, 1);
<a href="#l1596" class="line" id="l1596">   1596</a> 	XChangeProperty(dpy, wmcheckwin, netatom[NetWMName], utf8string, 8,
<a href="#l1597" class="line" id="l1597">   1597</a> 		PropModeReplace, (unsigned char *) &quot;dwm&quot;, 3);
<a href="#l1598" class="line" id="l1598">   1598</a> 	XChangeProperty(dpy, root, netatom[NetWMCheck], XA_WINDOW, 32,
<a href="#l1599" class="line" id="l1599">   1599</a> 		PropModeReplace, (unsigned char *) &amp;wmcheckwin, 1);
<a href="#l1600" class="line" id="l1600">   1600</a> 	/* EWMH support per view */
<a href="#l1601" class="line" id="l1601">   1601</a> 	XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
<a href="#l1602" class="line" id="l1602">   1602</a> 		PropModeReplace, (unsigned char *) netatom, NetLast);
<a href="#l1603" class="line" id="l1603">   1603</a> 	XDeleteProperty(dpy, root, netatom[NetClientList]);
<a href="#l1604" class="line" id="l1604">   1604</a> 	/* select events */
<a href="#l1605" class="line" id="l1605">   1605</a> 	wa.cursor = cursor[CurNormal]-&gt;cursor;
<a href="#l1606" class="line" id="l1606">   1606</a> 	wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask
<a href="#l1607" class="line" id="l1607">   1607</a> 		|ButtonPressMask|PointerMotionMask|EnterWindowMask
<a href="#l1608" class="line" id="l1608">   1608</a> 		|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
<a href="#l1609" class="line" id="l1609">   1609</a> 	XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &amp;wa);
<a href="#l1610" class="line" id="l1610">   1610</a> 	XSelectInput(dpy, root, wa.event_mask);
<a href="#l1611" class="line" id="l1611">   1611</a> 	grabkeys();
<a href="#l1612" class="line" id="l1612">   1612</a> 	focus(NULL);
<a href="#l1613" class="line" id="l1613">   1613</a> }
<a href="#l1614" class="line" id="l1614">   1614</a> 
<a href="#l1615" class="line" id="l1615">   1615</a> void
<a href="#l1616" class="line" id="l1616">   1616</a> seturgent(Client *c, int urg)
<a href="#l1617" class="line" id="l1617">   1617</a> {
<a href="#l1618" class="line" id="l1618">   1618</a> 	XWMHints *wmh;
<a href="#l1619" class="line" id="l1619">   1619</a> 
<a href="#l1620" class="line" id="l1620">   1620</a> 	c-&gt;isurgent = urg;
<a href="#l1621" class="line" id="l1621">   1621</a> 	if (!(wmh = XGetWMHints(dpy, c-&gt;win)))
<a href="#l1622" class="line" id="l1622">   1622</a> 		return;
<a href="#l1623" class="line" id="l1623">   1623</a> 	wmh-&gt;flags = urg ? (wmh-&gt;flags | XUrgencyHint) : (wmh-&gt;flags &amp; ~XUrgencyHint);
<a href="#l1624" class="line" id="l1624">   1624</a> 	XSetWMHints(dpy, c-&gt;win, wmh);
<a href="#l1625" class="line" id="l1625">   1625</a> 	XFree(wmh);
<a href="#l1626" class="line" id="l1626">   1626</a> }
<a href="#l1627" class="line" id="l1627">   1627</a> 
<a href="#l1628" class="line" id="l1628">   1628</a> void
<a href="#l1629" class="line" id="l1629">   1629</a> showhide(Client *c)
<a href="#l1630" class="line" id="l1630">   1630</a> {
<a href="#l1631" class="line" id="l1631">   1631</a> 	if (!c)
<a href="#l1632" class="line" id="l1632">   1632</a> 		return;
<a href="#l1633" class="line" id="l1633">   1633</a> 	if (ISVISIBLE(c)) {
<a href="#l1634" class="line" id="l1634">   1634</a> 		/* show clients top down */
<a href="#l1635" class="line" id="l1635">   1635</a> 		XMoveWindow(dpy, c-&gt;win, c-&gt;x, c-&gt;y);
<a href="#l1636" class="line" id="l1636">   1636</a> 		if ((!c-&gt;mon-&gt;lt[c-&gt;mon-&gt;sellt]-&gt;arrange || c-&gt;isfloating) &amp;&amp; !c-&gt;isfullscreen)
<a href="#l1637" class="line" id="l1637">   1637</a> 			resize(c, c-&gt;x, c-&gt;y, c-&gt;w, c-&gt;h, 0);
<a href="#l1638" class="line" id="l1638">   1638</a> 		showhide(c-&gt;snext);
<a href="#l1639" class="line" id="l1639">   1639</a> 	} else {
<a href="#l1640" class="line" id="l1640">   1640</a> 		/* hide clients bottom up */
<a href="#l1641" class="line" id="l1641">   1641</a> 		showhide(c-&gt;snext);
<a href="#l1642" class="line" id="l1642">   1642</a> 		XMoveWindow(dpy, c-&gt;win, WIDTH(c) * -2, c-&gt;y);
<a href="#l1643" class="line" id="l1643">   1643</a> 	}
<a href="#l1644" class="line" id="l1644">   1644</a> }
<a href="#l1645" class="line" id="l1645">   1645</a> 
<a href="#l1646" class="line" id="l1646">   1646</a> void
<a href="#l1647" class="line" id="l1647">   1647</a> spawn(const Arg *arg)
<a href="#l1648" class="line" id="l1648">   1648</a> {
<a href="#l1649" class="line" id="l1649">   1649</a> 	struct sigaction sa;
<a href="#l1650" class="line" id="l1650">   1650</a> 
<a href="#l1651" class="line" id="l1651">   1651</a> 	if (arg-&gt;v == dmenucmd)
<a href="#l1652" class="line" id="l1652">   1652</a> 		dmenumon[0] = &#39;0&#39; + selmon-&gt;num;
<a href="#l1653" class="line" id="l1653">   1653</a> 	if (fork() == 0) {
<a href="#l1654" class="line" id="l1654">   1654</a> 		if (dpy)
<a href="#l1655" class="line" id="l1655">   1655</a> 			close(ConnectionNumber(dpy));
<a href="#l1656" class="line" id="l1656">   1656</a> 		setsid();
<a href="#l1657" class="line" id="l1657">   1657</a> 
<a href="#l1658" class="line" id="l1658">   1658</a> 		sigemptyset(&amp;sa.sa_mask);
<a href="#l1659" class="line" id="l1659">   1659</a> 		sa.sa_flags = 0;
<a href="#l1660" class="line" id="l1660">   1660</a> 		sa.sa_handler = SIG_DFL;
<a href="#l1661" class="line" id="l1661">   1661</a> 		sigaction(SIGCHLD, &amp;sa, NULL);
<a href="#l1662" class="line" id="l1662">   1662</a> 
<a href="#l1663" class="line" id="l1663">   1663</a> 		execvp(((char **)arg-&gt;v)[0], (char **)arg-&gt;v);
<a href="#l1664" class="line" id="l1664">   1664</a> 		die(&quot;dwm: execvp &#39;%s&#39; failed:&quot;, ((char **)arg-&gt;v)[0]);
<a href="#l1665" class="line" id="l1665">   1665</a> 	}
<a href="#l1666" class="line" id="l1666">   1666</a> }
<a href="#l1667" class="line" id="l1667">   1667</a> 
<a href="#l1668" class="line" id="l1668">   1668</a> void
<a href="#l1669" class="line" id="l1669">   1669</a> tag(const Arg *arg)
<a href="#l1670" class="line" id="l1670">   1670</a> {
<a href="#l1671" class="line" id="l1671">   1671</a> 	if (selmon-&gt;sel &amp;&amp; arg-&gt;ui &amp; TAGMASK) {
<a href="#l1672" class="line" id="l1672">   1672</a> 		selmon-&gt;sel-&gt;tags = arg-&gt;ui &amp; TAGMASK;
<a href="#l1673" class="line" id="l1673">   1673</a> 		focus(NULL);
<a href="#l1674" class="line" id="l1674">   1674</a> 		arrange(selmon);
<a href="#l1675" class="line" id="l1675">   1675</a> 	}
<a href="#l1676" class="line" id="l1676">   1676</a> }
<a href="#l1677" class="line" id="l1677">   1677</a> 
<a href="#l1678" class="line" id="l1678">   1678</a> void
<a href="#l1679" class="line" id="l1679">   1679</a> tagmon(const Arg *arg)
<a href="#l1680" class="line" id="l1680">   1680</a> {
<a href="#l1681" class="line" id="l1681">   1681</a> 	if (!selmon-&gt;sel || !mons-&gt;next)
<a href="#l1682" class="line" id="l1682">   1682</a> 		return;
<a href="#l1683" class="line" id="l1683">   1683</a> 	sendmon(selmon-&gt;sel, dirtomon(arg-&gt;i));
<a href="#l1684" class="line" id="l1684">   1684</a> }
<a href="#l1685" class="line" id="l1685">   1685</a> 
<a href="#l1686" class="line" id="l1686">   1686</a> void
<a href="#l1687" class="line" id="l1687">   1687</a> tile(Monitor *m)
<a href="#l1688" class="line" id="l1688">   1688</a> {
<a href="#l1689" class="line" id="l1689">   1689</a> 	unsigned int i, n, h, mw, my, ty;
<a href="#l1690" class="line" id="l1690">   1690</a> 	Client *c;
<a href="#l1691" class="line" id="l1691">   1691</a> 
<a href="#l1692" class="line" id="l1692">   1692</a> 	for (n = 0, c = nexttiled(m-&gt;clients); c; c = nexttiled(c-&gt;next), n++);
<a href="#l1693" class="line" id="l1693">   1693</a> 	if (n == 0)
<a href="#l1694" class="line" id="l1694">   1694</a> 		return;
<a href="#l1695" class="line" id="l1695">   1695</a> 
<a href="#l1696" class="line" id="l1696">   1696</a> 	if (n &gt; m-&gt;nmaster)
<a href="#l1697" class="line" id="l1697">   1697</a> 		mw = m-&gt;nmaster ? m-&gt;ww * m-&gt;mfact : 0;
<a href="#l1698" class="line" id="l1698">   1698</a> 	else
<a href="#l1699" class="line" id="l1699">   1699</a> 		mw = m-&gt;ww;
<a href="#l1700" class="line" id="l1700">   1700</a> 	for (i = my = ty = 0, c = nexttiled(m-&gt;clients); c; c = nexttiled(c-&gt;next), i++)
<a href="#l1701" class="line" id="l1701">   1701</a> 		if (i &lt; m-&gt;nmaster) {
<a href="#l1702" class="line" id="l1702">   1702</a> 			h = (m-&gt;wh - my) / (MIN(n, m-&gt;nmaster) - i);
<a href="#l1703" class="line" id="l1703">   1703</a> 			resize(c, m-&gt;wx, m-&gt;wy + my, mw - (2*c-&gt;bw), h - (2*c-&gt;bw), 0);
<a href="#l1704" class="line" id="l1704">   1704</a> 			if (my + HEIGHT(c) &lt; m-&gt;wh)
<a href="#l1705" class="line" id="l1705">   1705</a> 				my += HEIGHT(c);
<a href="#l1706" class="line" id="l1706">   1706</a> 		} else {
<a href="#l1707" class="line" id="l1707">   1707</a> 			h = (m-&gt;wh - ty) / (n - i);
<a href="#l1708" class="line" id="l1708">   1708</a> 			resize(c, m-&gt;wx + mw, m-&gt;wy + ty, m-&gt;ww - mw - (2*c-&gt;bw), h - (2*c-&gt;bw), 0);
<a href="#l1709" class="line" id="l1709">   1709</a> 			if (ty + HEIGHT(c) &lt; m-&gt;wh)
<a href="#l1710" class="line" id="l1710">   1710</a> 				ty += HEIGHT(c);
<a href="#l1711" class="line" id="l1711">   1711</a> 		}
<a href="#l1712" class="line" id="l1712">   1712</a> }
<a href="#l1713" class="line" id="l1713">   1713</a> 
<a href="#l1714" class="line" id="l1714">   1714</a> void
<a href="#l1715" class="line" id="l1715">   1715</a> togglebar(const Arg *arg)
<a href="#l1716" class="line" id="l1716">   1716</a> {
<a href="#l1717" class="line" id="l1717">   1717</a> 	selmon-&gt;showbar = !selmon-&gt;showbar;
<a href="#l1718" class="line" id="l1718">   1718</a> 	updatebarpos(selmon);
<a href="#l1719" class="line" id="l1719">   1719</a> 	XMoveResizeWindow(dpy, selmon-&gt;barwin, selmon-&gt;wx, selmon-&gt;by, selmon-&gt;ww, bh);
<a href="#l1720" class="line" id="l1720">   1720</a> 	arrange(selmon);
<a href="#l1721" class="line" id="l1721">   1721</a> }
<a href="#l1722" class="line" id="l1722">   1722</a> 
<a href="#l1723" class="line" id="l1723">   1723</a> void
<a href="#l1724" class="line" id="l1724">   1724</a> togglefloating(const Arg *arg)
<a href="#l1725" class="line" id="l1725">   1725</a> {
<a href="#l1726" class="line" id="l1726">   1726</a> 	if (!selmon-&gt;sel)
<a href="#l1727" class="line" id="l1727">   1727</a> 		return;
<a href="#l1728" class="line" id="l1728">   1728</a> 	if (selmon-&gt;sel-&gt;isfullscreen) /* no support for fullscreen windows */
<a href="#l1729" class="line" id="l1729">   1729</a> 		return;
<a href="#l1730" class="line" id="l1730">   1730</a> 	selmon-&gt;sel-&gt;isfloating = !selmon-&gt;sel-&gt;isfloating || selmon-&gt;sel-&gt;isfixed;
<a href="#l1731" class="line" id="l1731">   1731</a> 	if (selmon-&gt;sel-&gt;isfloating)
<a href="#l1732" class="line" id="l1732">   1732</a> 		resize(selmon-&gt;sel, selmon-&gt;sel-&gt;x, selmon-&gt;sel-&gt;y,
<a href="#l1733" class="line" id="l1733">   1733</a> 			selmon-&gt;sel-&gt;w, selmon-&gt;sel-&gt;h, 0);
<a href="#l1734" class="line" id="l1734">   1734</a> 	arrange(selmon);
<a href="#l1735" class="line" id="l1735">   1735</a> }
<a href="#l1736" class="line" id="l1736">   1736</a> 
<a href="#l1737" class="line" id="l1737">   1737</a> void
<a href="#l1738" class="line" id="l1738">   1738</a> toggletag(const Arg *arg)
<a href="#l1739" class="line" id="l1739">   1739</a> {
<a href="#l1740" class="line" id="l1740">   1740</a> 	unsigned int newtags;
<a href="#l1741" class="line" id="l1741">   1741</a> 
<a href="#l1742" class="line" id="l1742">   1742</a> 	if (!selmon-&gt;sel)
<a href="#l1743" class="line" id="l1743">   1743</a> 		return;
<a href="#l1744" class="line" id="l1744">   1744</a> 	newtags = selmon-&gt;sel-&gt;tags ^ (arg-&gt;ui &amp; TAGMASK);
<a href="#l1745" class="line" id="l1745">   1745</a> 	if (newtags) {
<a href="#l1746" class="line" id="l1746">   1746</a> 		selmon-&gt;sel-&gt;tags = newtags;
<a href="#l1747" class="line" id="l1747">   1747</a> 		focus(NULL);
<a href="#l1748" class="line" id="l1748">   1748</a> 		arrange(selmon);
<a href="#l1749" class="line" id="l1749">   1749</a> 	}
<a href="#l1750" class="line" id="l1750">   1750</a> }
<a href="#l1751" class="line" id="l1751">   1751</a> 
<a href="#l1752" class="line" id="l1752">   1752</a> void
<a href="#l1753" class="line" id="l1753">   1753</a> toggleview(const Arg *arg)
<a href="#l1754" class="line" id="l1754">   1754</a> {
<a href="#l1755" class="line" id="l1755">   1755</a> 	unsigned int newtagset = selmon-&gt;tagset[selmon-&gt;seltags] ^ (arg-&gt;ui &amp; TAGMASK);
<a href="#l1756" class="line" id="l1756">   1756</a> 
<a href="#l1757" class="line" id="l1757">   1757</a> 	if (newtagset) {
<a href="#l1758" class="line" id="l1758">   1758</a> 		selmon-&gt;tagset[selmon-&gt;seltags] = newtagset;
<a href="#l1759" class="line" id="l1759">   1759</a> 		focus(NULL);
<a href="#l1760" class="line" id="l1760">   1760</a> 		arrange(selmon);
<a href="#l1761" class="line" id="l1761">   1761</a> 	}
<a href="#l1762" class="line" id="l1762">   1762</a> }
<a href="#l1763" class="line" id="l1763">   1763</a> 
<a href="#l1764" class="line" id="l1764">   1764</a> void
<a href="#l1765" class="line" id="l1765">   1765</a> unfocus(Client *c, int setfocus)
<a href="#l1766" class="line" id="l1766">   1766</a> {
<a href="#l1767" class="line" id="l1767">   1767</a> 	if (!c)
<a href="#l1768" class="line" id="l1768">   1768</a> 		return;
<a href="#l1769" class="line" id="l1769">   1769</a> 	grabbuttons(c, 0);
<a href="#l1770" class="line" id="l1770">   1770</a> 	XSetWindowBorder(dpy, c-&gt;win, scheme[SchemeNorm][ColBorder].pixel);
<a href="#l1771" class="line" id="l1771">   1771</a> 	if (setfocus) {
<a href="#l1772" class="line" id="l1772">   1772</a> 		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
<a href="#l1773" class="line" id="l1773">   1773</a> 		XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
<a href="#l1774" class="line" id="l1774">   1774</a> 	}
<a href="#l1775" class="line" id="l1775">   1775</a> }
<a href="#l1776" class="line" id="l1776">   1776</a> 
<a href="#l1777" class="line" id="l1777">   1777</a> void
<a href="#l1778" class="line" id="l1778">   1778</a> unmanage(Client *c, int destroyed)
<a href="#l1779" class="line" id="l1779">   1779</a> {
<a href="#l1780" class="line" id="l1780">   1780</a> 	Monitor *m = c-&gt;mon;
<a href="#l1781" class="line" id="l1781">   1781</a> 	XWindowChanges wc;
<a href="#l1782" class="line" id="l1782">   1782</a> 
<a href="#l1783" class="line" id="l1783">   1783</a> 	detach(c);
<a href="#l1784" class="line" id="l1784">   1784</a> 	detachstack(c);
<a href="#l1785" class="line" id="l1785">   1785</a> 	if (!destroyed) {
<a href="#l1786" class="line" id="l1786">   1786</a> 		wc.border_width = c-&gt;oldbw;
<a href="#l1787" class="line" id="l1787">   1787</a> 		XGrabServer(dpy); /* avoid race conditions */
<a href="#l1788" class="line" id="l1788">   1788</a> 		XSetErrorHandler(xerrordummy);
<a href="#l1789" class="line" id="l1789">   1789</a> 		XSelectInput(dpy, c-&gt;win, NoEventMask);
<a href="#l1790" class="line" id="l1790">   1790</a> 		XConfigureWindow(dpy, c-&gt;win, CWBorderWidth, &amp;wc); /* restore border */
<a href="#l1791" class="line" id="l1791">   1791</a> 		XUngrabButton(dpy, AnyButton, AnyModifier, c-&gt;win);
<a href="#l1792" class="line" id="l1792">   1792</a> 		setclientstate(c, WithdrawnState);
<a href="#l1793" class="line" id="l1793">   1793</a> 		XSync(dpy, False);
<a href="#l1794" class="line" id="l1794">   1794</a> 		XSetErrorHandler(xerror);
<a href="#l1795" class="line" id="l1795">   1795</a> 		XUngrabServer(dpy);
<a href="#l1796" class="line" id="l1796">   1796</a> 	}
<a href="#l1797" class="line" id="l1797">   1797</a> 	free(c);
<a href="#l1798" class="line" id="l1798">   1798</a> 	focus(NULL);
<a href="#l1799" class="line" id="l1799">   1799</a> 	updateclientlist();
<a href="#l1800" class="line" id="l1800">   1800</a> 	arrange(m);
<a href="#l1801" class="line" id="l1801">   1801</a> }
<a href="#l1802" class="line" id="l1802">   1802</a> 
<a href="#l1803" class="line" id="l1803">   1803</a> void
<a href="#l1804" class="line" id="l1804">   1804</a> unmapnotify(XEvent *e)
<a href="#l1805" class="line" id="l1805">   1805</a> {
<a href="#l1806" class="line" id="l1806">   1806</a> 	Client *c;
<a href="#l1807" class="line" id="l1807">   1807</a> 	XUnmapEvent *ev = &amp;e-&gt;xunmap;
<a href="#l1808" class="line" id="l1808">   1808</a> 
<a href="#l1809" class="line" id="l1809">   1809</a> 	if ((c = wintoclient(ev-&gt;window))) {
<a href="#l1810" class="line" id="l1810">   1810</a> 		if (ev-&gt;send_event)
<a href="#l1811" class="line" id="l1811">   1811</a> 			setclientstate(c, WithdrawnState);
<a href="#l1812" class="line" id="l1812">   1812</a> 		else
<a href="#l1813" class="line" id="l1813">   1813</a> 			unmanage(c, 0);
<a href="#l1814" class="line" id="l1814">   1814</a> 	}
<a href="#l1815" class="line" id="l1815">   1815</a> }
<a href="#l1816" class="line" id="l1816">   1816</a> 
<a href="#l1817" class="line" id="l1817">   1817</a> void
<a href="#l1818" class="line" id="l1818">   1818</a> updatebars(void)
<a href="#l1819" class="line" id="l1819">   1819</a> {
<a href="#l1820" class="line" id="l1820">   1820</a> 	Monitor *m;
<a href="#l1821" class="line" id="l1821">   1821</a> 	XSetWindowAttributes wa = {
<a href="#l1822" class="line" id="l1822">   1822</a> 		.override_redirect = True,
<a href="#l1823" class="line" id="l1823">   1823</a> 		.background_pixmap = ParentRelative,
<a href="#l1824" class="line" id="l1824">   1824</a> 		.event_mask = ButtonPressMask|ExposureMask
<a href="#l1825" class="line" id="l1825">   1825</a> 	};
<a href="#l1826" class="line" id="l1826">   1826</a> 	XClassHint ch = {&quot;dwm&quot;, &quot;dwm&quot;};
<a href="#l1827" class="line" id="l1827">   1827</a> 	for (m = mons; m; m = m-&gt;next) {
<a href="#l1828" class="line" id="l1828">   1828</a> 		if (m-&gt;barwin)
<a href="#l1829" class="line" id="l1829">   1829</a> 			continue;
<a href="#l1830" class="line" id="l1830">   1830</a> 		m-&gt;barwin = XCreateWindow(dpy, root, m-&gt;wx, m-&gt;by, m-&gt;ww, bh, 0, DefaultDepth(dpy, screen),
<a href="#l1831" class="line" id="l1831">   1831</a> 				CopyFromParent, DefaultVisual(dpy, screen),
<a href="#l1832" class="line" id="l1832">   1832</a> 				CWOverrideRedirect|CWBackPixmap|CWEventMask, &amp;wa);
<a href="#l1833" class="line" id="l1833">   1833</a> 		XDefineCursor(dpy, m-&gt;barwin, cursor[CurNormal]-&gt;cursor);
<a href="#l1834" class="line" id="l1834">   1834</a> 		XMapRaised(dpy, m-&gt;barwin);
<a href="#l1835" class="line" id="l1835">   1835</a> 		XSetClassHint(dpy, m-&gt;barwin, &amp;ch);
<a href="#l1836" class="line" id="l1836">   1836</a> 	}
<a href="#l1837" class="line" id="l1837">   1837</a> }
<a href="#l1838" class="line" id="l1838">   1838</a> 
<a href="#l1839" class="line" id="l1839">   1839</a> void
<a href="#l1840" class="line" id="l1840">   1840</a> updatebarpos(Monitor *m)
<a href="#l1841" class="line" id="l1841">   1841</a> {
<a href="#l1842" class="line" id="l1842">   1842</a> 	m-&gt;wy = m-&gt;my;
<a href="#l1843" class="line" id="l1843">   1843</a> 	m-&gt;wh = m-&gt;mh;
<a href="#l1844" class="line" id="l1844">   1844</a> 	if (m-&gt;showbar) {
<a href="#l1845" class="line" id="l1845">   1845</a> 		m-&gt;wh -= bh;
<a href="#l1846" class="line" id="l1846">   1846</a> 		m-&gt;by = m-&gt;topbar ? m-&gt;wy : m-&gt;wy + m-&gt;wh;
<a href="#l1847" class="line" id="l1847">   1847</a> 		m-&gt;wy = m-&gt;topbar ? m-&gt;wy + bh : m-&gt;wy;
<a href="#l1848" class="line" id="l1848">   1848</a> 	} else
<a href="#l1849" class="line" id="l1849">   1849</a> 		m-&gt;by = -bh;
<a href="#l1850" class="line" id="l1850">   1850</a> }
<a href="#l1851" class="line" id="l1851">   1851</a> 
<a href="#l1852" class="line" id="l1852">   1852</a> void
<a href="#l1853" class="line" id="l1853">   1853</a> updateclientlist(void)
<a href="#l1854" class="line" id="l1854">   1854</a> {
<a href="#l1855" class="line" id="l1855">   1855</a> 	Client *c;
<a href="#l1856" class="line" id="l1856">   1856</a> 	Monitor *m;
<a href="#l1857" class="line" id="l1857">   1857</a> 
<a href="#l1858" class="line" id="l1858">   1858</a> 	XDeleteProperty(dpy, root, netatom[NetClientList]);
<a href="#l1859" class="line" id="l1859">   1859</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l1860" class="line" id="l1860">   1860</a> 		for (c = m-&gt;clients; c; c = c-&gt;next)
<a href="#l1861" class="line" id="l1861">   1861</a> 			XChangeProperty(dpy, root, netatom[NetClientList],
<a href="#l1862" class="line" id="l1862">   1862</a> 				XA_WINDOW, 32, PropModeAppend,
<a href="#l1863" class="line" id="l1863">   1863</a> 				(unsigned char *) &amp;(c-&gt;win), 1);
<a href="#l1864" class="line" id="l1864">   1864</a> }
<a href="#l1865" class="line" id="l1865">   1865</a> 
<a href="#l1866" class="line" id="l1866">   1866</a> int
<a href="#l1867" class="line" id="l1867">   1867</a> updategeom(void)
<a href="#l1868" class="line" id="l1868">   1868</a> {
<a href="#l1869" class="line" id="l1869">   1869</a> 	int dirty = 0;
<a href="#l1870" class="line" id="l1870">   1870</a> 
<a href="#l1871" class="line" id="l1871">   1871</a> #ifdef XINERAMA
<a href="#l1872" class="line" id="l1872">   1872</a> 	if (XineramaIsActive(dpy)) {
<a href="#l1873" class="line" id="l1873">   1873</a> 		int i, j, n, nn;
<a href="#l1874" class="line" id="l1874">   1874</a> 		Client *c;
<a href="#l1875" class="line" id="l1875">   1875</a> 		Monitor *m;
<a href="#l1876" class="line" id="l1876">   1876</a> 		XineramaScreenInfo *info = XineramaQueryScreens(dpy, &amp;nn);
<a href="#l1877" class="line" id="l1877">   1877</a> 		XineramaScreenInfo *unique = NULL;
<a href="#l1878" class="line" id="l1878">   1878</a> 
<a href="#l1879" class="line" id="l1879">   1879</a> 		for (n = 0, m = mons; m; m = m-&gt;next, n++);
<a href="#l1880" class="line" id="l1880">   1880</a> 		/* only consider unique geometries as separate screens */
<a href="#l1881" class="line" id="l1881">   1881</a> 		unique = ecalloc(nn, sizeof(XineramaScreenInfo));
<a href="#l1882" class="line" id="l1882">   1882</a> 		for (i = 0, j = 0; i &lt; nn; i++)
<a href="#l1883" class="line" id="l1883">   1883</a> 			if (isuniquegeom(unique, j, &amp;info[i]))
<a href="#l1884" class="line" id="l1884">   1884</a> 				memcpy(&amp;unique[j++], &amp;info[i], sizeof(XineramaScreenInfo));
<a href="#l1885" class="line" id="l1885">   1885</a> 		XFree(info);
<a href="#l1886" class="line" id="l1886">   1886</a> 		nn = j;
<a href="#l1887" class="line" id="l1887">   1887</a> 
<a href="#l1888" class="line" id="l1888">   1888</a> 		/* new monitors if nn &gt; n */
<a href="#l1889" class="line" id="l1889">   1889</a> 		for (i = n; i &lt; nn; i++) {
<a href="#l1890" class="line" id="l1890">   1890</a> 			for (m = mons; m &amp;&amp; m-&gt;next; m = m-&gt;next);
<a href="#l1891" class="line" id="l1891">   1891</a> 			if (m)
<a href="#l1892" class="line" id="l1892">   1892</a> 				m-&gt;next = createmon();
<a href="#l1893" class="line" id="l1893">   1893</a> 			else
<a href="#l1894" class="line" id="l1894">   1894</a> 				mons = createmon();
<a href="#l1895" class="line" id="l1895">   1895</a> 		}
<a href="#l1896" class="line" id="l1896">   1896</a> 		for (i = 0, m = mons; i &lt; nn &amp;&amp; m; m = m-&gt;next, i++)
<a href="#l1897" class="line" id="l1897">   1897</a> 			if (i &gt;= n
<a href="#l1898" class="line" id="l1898">   1898</a> 			|| unique[i].x_org != m-&gt;mx || unique[i].y_org != m-&gt;my
<a href="#l1899" class="line" id="l1899">   1899</a> 			|| unique[i].width != m-&gt;mw || unique[i].height != m-&gt;mh)
<a href="#l1900" class="line" id="l1900">   1900</a> 			{
<a href="#l1901" class="line" id="l1901">   1901</a> 				dirty = 1;
<a href="#l1902" class="line" id="l1902">   1902</a> 				m-&gt;num = i;
<a href="#l1903" class="line" id="l1903">   1903</a> 				m-&gt;mx = m-&gt;wx = unique[i].x_org;
<a href="#l1904" class="line" id="l1904">   1904</a> 				m-&gt;my = m-&gt;wy = unique[i].y_org;
<a href="#l1905" class="line" id="l1905">   1905</a> 				m-&gt;mw = m-&gt;ww = unique[i].width;
<a href="#l1906" class="line" id="l1906">   1906</a> 				m-&gt;mh = m-&gt;wh = unique[i].height;
<a href="#l1907" class="line" id="l1907">   1907</a> 				updatebarpos(m);
<a href="#l1908" class="line" id="l1908">   1908</a> 			}
<a href="#l1909" class="line" id="l1909">   1909</a> 		/* removed monitors if n &gt; nn */
<a href="#l1910" class="line" id="l1910">   1910</a> 		for (i = nn; i &lt; n; i++) {
<a href="#l1911" class="line" id="l1911">   1911</a> 			for (m = mons; m &amp;&amp; m-&gt;next; m = m-&gt;next);
<a href="#l1912" class="line" id="l1912">   1912</a> 			while ((c = m-&gt;clients)) {
<a href="#l1913" class="line" id="l1913">   1913</a> 				dirty = 1;
<a href="#l1914" class="line" id="l1914">   1914</a> 				m-&gt;clients = c-&gt;next;
<a href="#l1915" class="line" id="l1915">   1915</a> 				detachstack(c);
<a href="#l1916" class="line" id="l1916">   1916</a> 				c-&gt;mon = mons;
<a href="#l1917" class="line" id="l1917">   1917</a> 				attach(c);
<a href="#l1918" class="line" id="l1918">   1918</a> 				attachstack(c);
<a href="#l1919" class="line" id="l1919">   1919</a> 			}
<a href="#l1920" class="line" id="l1920">   1920</a> 			if (m == selmon)
<a href="#l1921" class="line" id="l1921">   1921</a> 				selmon = mons;
<a href="#l1922" class="line" id="l1922">   1922</a> 			cleanupmon(m);
<a href="#l1923" class="line" id="l1923">   1923</a> 		}
<a href="#l1924" class="line" id="l1924">   1924</a> 		free(unique);
<a href="#l1925" class="line" id="l1925">   1925</a> 	} else
<a href="#l1926" class="line" id="l1926">   1926</a> #endif /* XINERAMA */
<a href="#l1927" class="line" id="l1927">   1927</a> 	{ /* default monitor setup */
<a href="#l1928" class="line" id="l1928">   1928</a> 		if (!mons)
<a href="#l1929" class="line" id="l1929">   1929</a> 			mons = createmon();
<a href="#l1930" class="line" id="l1930">   1930</a> 		if (mons-&gt;mw != sw || mons-&gt;mh != sh) {
<a href="#l1931" class="line" id="l1931">   1931</a> 			dirty = 1;
<a href="#l1932" class="line" id="l1932">   1932</a> 			mons-&gt;mw = mons-&gt;ww = sw;
<a href="#l1933" class="line" id="l1933">   1933</a> 			mons-&gt;mh = mons-&gt;wh = sh;
<a href="#l1934" class="line" id="l1934">   1934</a> 			updatebarpos(mons);
<a href="#l1935" class="line" id="l1935">   1935</a> 		}
<a href="#l1936" class="line" id="l1936">   1936</a> 	}
<a href="#l1937" class="line" id="l1937">   1937</a> 	if (dirty) {
<a href="#l1938" class="line" id="l1938">   1938</a> 		selmon = mons;
<a href="#l1939" class="line" id="l1939">   1939</a> 		selmon = wintomon(root);
<a href="#l1940" class="line" id="l1940">   1940</a> 	}
<a href="#l1941" class="line" id="l1941">   1941</a> 	return dirty;
<a href="#l1942" class="line" id="l1942">   1942</a> }
<a href="#l1943" class="line" id="l1943">   1943</a> 
<a href="#l1944" class="line" id="l1944">   1944</a> void
<a href="#l1945" class="line" id="l1945">   1945</a> updatenumlockmask(void)
<a href="#l1946" class="line" id="l1946">   1946</a> {
<a href="#l1947" class="line" id="l1947">   1947</a> 	unsigned int i, j;
<a href="#l1948" class="line" id="l1948">   1948</a> 	XModifierKeymap *modmap;
<a href="#l1949" class="line" id="l1949">   1949</a> 
<a href="#l1950" class="line" id="l1950">   1950</a> 	numlockmask = 0;
<a href="#l1951" class="line" id="l1951">   1951</a> 	modmap = XGetModifierMapping(dpy);
<a href="#l1952" class="line" id="l1952">   1952</a> 	for (i = 0; i &lt; 8; i++)
<a href="#l1953" class="line" id="l1953">   1953</a> 		for (j = 0; j &lt; modmap-&gt;max_keypermod; j++)
<a href="#l1954" class="line" id="l1954">   1954</a> 			if (modmap-&gt;modifiermap[i * modmap-&gt;max_keypermod + j]
<a href="#l1955" class="line" id="l1955">   1955</a> 				== XKeysymToKeycode(dpy, XK_Num_Lock))
<a href="#l1956" class="line" id="l1956">   1956</a> 				numlockmask = (1 &lt;&lt; i);
<a href="#l1957" class="line" id="l1957">   1957</a> 	XFreeModifiermap(modmap);
<a href="#l1958" class="line" id="l1958">   1958</a> }
<a href="#l1959" class="line" id="l1959">   1959</a> 
<a href="#l1960" class="line" id="l1960">   1960</a> void
<a href="#l1961" class="line" id="l1961">   1961</a> updatesizehints(Client *c)
<a href="#l1962" class="line" id="l1962">   1962</a> {
<a href="#l1963" class="line" id="l1963">   1963</a> 	long msize;
<a href="#l1964" class="line" id="l1964">   1964</a> 	XSizeHints size;
<a href="#l1965" class="line" id="l1965">   1965</a> 
<a href="#l1966" class="line" id="l1966">   1966</a> 	if (!XGetWMNormalHints(dpy, c-&gt;win, &amp;size, &amp;msize))
<a href="#l1967" class="line" id="l1967">   1967</a> 		/* size is uninitialized, ensure that size.flags aren&#39;t used */
<a href="#l1968" class="line" id="l1968">   1968</a> 		size.flags = PSize;
<a href="#l1969" class="line" id="l1969">   1969</a> 	if (size.flags &amp; PBaseSize) {
<a href="#l1970" class="line" id="l1970">   1970</a> 		c-&gt;basew = size.base_width;
<a href="#l1971" class="line" id="l1971">   1971</a> 		c-&gt;baseh = size.base_height;
<a href="#l1972" class="line" id="l1972">   1972</a> 	} else if (size.flags &amp; PMinSize) {
<a href="#l1973" class="line" id="l1973">   1973</a> 		c-&gt;basew = size.min_width;
<a href="#l1974" class="line" id="l1974">   1974</a> 		c-&gt;baseh = size.min_height;
<a href="#l1975" class="line" id="l1975">   1975</a> 	} else
<a href="#l1976" class="line" id="l1976">   1976</a> 		c-&gt;basew = c-&gt;baseh = 0;
<a href="#l1977" class="line" id="l1977">   1977</a> 	if (size.flags &amp; PResizeInc) {
<a href="#l1978" class="line" id="l1978">   1978</a> 		c-&gt;incw = size.width_inc;
<a href="#l1979" class="line" id="l1979">   1979</a> 		c-&gt;inch = size.height_inc;
<a href="#l1980" class="line" id="l1980">   1980</a> 	} else
<a href="#l1981" class="line" id="l1981">   1981</a> 		c-&gt;incw = c-&gt;inch = 0;
<a href="#l1982" class="line" id="l1982">   1982</a> 	if (size.flags &amp; PMaxSize) {
<a href="#l1983" class="line" id="l1983">   1983</a> 		c-&gt;maxw = size.max_width;
<a href="#l1984" class="line" id="l1984">   1984</a> 		c-&gt;maxh = size.max_height;
<a href="#l1985" class="line" id="l1985">   1985</a> 	} else
<a href="#l1986" class="line" id="l1986">   1986</a> 		c-&gt;maxw = c-&gt;maxh = 0;
<a href="#l1987" class="line" id="l1987">   1987</a> 	if (size.flags &amp; PMinSize) {
<a href="#l1988" class="line" id="l1988">   1988</a> 		c-&gt;minw = size.min_width;
<a href="#l1989" class="line" id="l1989">   1989</a> 		c-&gt;minh = size.min_height;
<a href="#l1990" class="line" id="l1990">   1990</a> 	} else if (size.flags &amp; PBaseSize) {
<a href="#l1991" class="line" id="l1991">   1991</a> 		c-&gt;minw = size.base_width;
<a href="#l1992" class="line" id="l1992">   1992</a> 		c-&gt;minh = size.base_height;
<a href="#l1993" class="line" id="l1993">   1993</a> 	} else
<a href="#l1994" class="line" id="l1994">   1994</a> 		c-&gt;minw = c-&gt;minh = 0;
<a href="#l1995" class="line" id="l1995">   1995</a> 	if (size.flags &amp; PAspect) {
<a href="#l1996" class="line" id="l1996">   1996</a> 		c-&gt;mina = (float)size.min_aspect.y / size.min_aspect.x;
<a href="#l1997" class="line" id="l1997">   1997</a> 		c-&gt;maxa = (float)size.max_aspect.x / size.max_aspect.y;
<a href="#l1998" class="line" id="l1998">   1998</a> 	} else
<a href="#l1999" class="line" id="l1999">   1999</a> 		c-&gt;maxa = c-&gt;mina = 0.0;
<a href="#l2000" class="line" id="l2000">   2000</a> 	c-&gt;isfixed = (c-&gt;maxw &amp;&amp; c-&gt;maxh &amp;&amp; c-&gt;maxw == c-&gt;minw &amp;&amp; c-&gt;maxh == c-&gt;minh);
<a href="#l2001" class="line" id="l2001">   2001</a> 	c-&gt;hintsvalid = 1;
<a href="#l2002" class="line" id="l2002">   2002</a> }
<a href="#l2003" class="line" id="l2003">   2003</a> 
<a href="#l2004" class="line" id="l2004">   2004</a> void
<a href="#l2005" class="line" id="l2005">   2005</a> updatestatus(void)
<a href="#l2006" class="line" id="l2006">   2006</a> {
<a href="#l2007" class="line" id="l2007">   2007</a> 	if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext)))
<a href="#l2008" class="line" id="l2008">   2008</a> 		strcpy(stext, &quot;dwm-&quot;VERSION);
<a href="#l2009" class="line" id="l2009">   2009</a> 	drawbar(selmon);
<a href="#l2010" class="line" id="l2010">   2010</a> }
<a href="#l2011" class="line" id="l2011">   2011</a> 
<a href="#l2012" class="line" id="l2012">   2012</a> void
<a href="#l2013" class="line" id="l2013">   2013</a> updatetitle(Client *c)
<a href="#l2014" class="line" id="l2014">   2014</a> {
<a href="#l2015" class="line" id="l2015">   2015</a> 	if (!gettextprop(c-&gt;win, netatom[NetWMName], c-&gt;name, sizeof c-&gt;name))
<a href="#l2016" class="line" id="l2016">   2016</a> 		gettextprop(c-&gt;win, XA_WM_NAME, c-&gt;name, sizeof c-&gt;name);
<a href="#l2017" class="line" id="l2017">   2017</a> 	if (c-&gt;name[0] == &#39;\0&#39;) /* hack to mark broken clients */
<a href="#l2018" class="line" id="l2018">   2018</a> 		strcpy(c-&gt;name, broken);
<a href="#l2019" class="line" id="l2019">   2019</a> }
<a href="#l2020" class="line" id="l2020">   2020</a> 
<a href="#l2021" class="line" id="l2021">   2021</a> void
<a href="#l2022" class="line" id="l2022">   2022</a> updatewindowtype(Client *c)
<a href="#l2023" class="line" id="l2023">   2023</a> {
<a href="#l2024" class="line" id="l2024">   2024</a> 	Atom state = getatomprop(c, netatom[NetWMState]);
<a href="#l2025" class="line" id="l2025">   2025</a> 	Atom wtype = getatomprop(c, netatom[NetWMWindowType]);
<a href="#l2026" class="line" id="l2026">   2026</a> 
<a href="#l2027" class="line" id="l2027">   2027</a> 	if (state == netatom[NetWMFullscreen])
<a href="#l2028" class="line" id="l2028">   2028</a> 		setfullscreen(c, 1);
<a href="#l2029" class="line" id="l2029">   2029</a> 	if (wtype == netatom[NetWMWindowTypeDialog])
<a href="#l2030" class="line" id="l2030">   2030</a> 		c-&gt;isfloating = 1;
<a href="#l2031" class="line" id="l2031">   2031</a> }
<a href="#l2032" class="line" id="l2032">   2032</a> 
<a href="#l2033" class="line" id="l2033">   2033</a> void
<a href="#l2034" class="line" id="l2034">   2034</a> updatewmhints(Client *c)
<a href="#l2035" class="line" id="l2035">   2035</a> {
<a href="#l2036" class="line" id="l2036">   2036</a> 	XWMHints *wmh;
<a href="#l2037" class="line" id="l2037">   2037</a> 
<a href="#l2038" class="line" id="l2038">   2038</a> 	if ((wmh = XGetWMHints(dpy, c-&gt;win))) {
<a href="#l2039" class="line" id="l2039">   2039</a> 		if (c == selmon-&gt;sel &amp;&amp; wmh-&gt;flags &amp; XUrgencyHint) {
<a href="#l2040" class="line" id="l2040">   2040</a> 			wmh-&gt;flags &amp;= ~XUrgencyHint;
<a href="#l2041" class="line" id="l2041">   2041</a> 			XSetWMHints(dpy, c-&gt;win, wmh);
<a href="#l2042" class="line" id="l2042">   2042</a> 		} else
<a href="#l2043" class="line" id="l2043">   2043</a> 			c-&gt;isurgent = (wmh-&gt;flags &amp; XUrgencyHint) ? 1 : 0;
<a href="#l2044" class="line" id="l2044">   2044</a> 		if (wmh-&gt;flags &amp; InputHint)
<a href="#l2045" class="line" id="l2045">   2045</a> 			c-&gt;neverfocus = !wmh-&gt;input;
<a href="#l2046" class="line" id="l2046">   2046</a> 		else
<a href="#l2047" class="line" id="l2047">   2047</a> 			c-&gt;neverfocus = 0;
<a href="#l2048" class="line" id="l2048">   2048</a> 		XFree(wmh);
<a href="#l2049" class="line" id="l2049">   2049</a> 	}
<a href="#l2050" class="line" id="l2050">   2050</a> }
<a href="#l2051" class="line" id="l2051">   2051</a> 
<a href="#l2052" class="line" id="l2052">   2052</a> void
<a href="#l2053" class="line" id="l2053">   2053</a> view(const Arg *arg)
<a href="#l2054" class="line" id="l2054">   2054</a> {
<a href="#l2055" class="line" id="l2055">   2055</a> 	if ((arg-&gt;ui &amp; TAGMASK) == selmon-&gt;tagset[selmon-&gt;seltags])
<a href="#l2056" class="line" id="l2056">   2056</a> 		return;
<a href="#l2057" class="line" id="l2057">   2057</a> 	selmon-&gt;seltags ^= 1; /* toggle sel tagset */
<a href="#l2058" class="line" id="l2058">   2058</a> 	if (arg-&gt;ui &amp; TAGMASK)
<a href="#l2059" class="line" id="l2059">   2059</a> 		selmon-&gt;tagset[selmon-&gt;seltags] = arg-&gt;ui &amp; TAGMASK;
<a href="#l2060" class="line" id="l2060">   2060</a> 	focus(NULL);
<a href="#l2061" class="line" id="l2061">   2061</a> 	arrange(selmon);
<a href="#l2062" class="line" id="l2062">   2062</a> }
<a href="#l2063" class="line" id="l2063">   2063</a> 
<a href="#l2064" class="line" id="l2064">   2064</a> Client *
<a href="#l2065" class="line" id="l2065">   2065</a> wintoclient(Window w)
<a href="#l2066" class="line" id="l2066">   2066</a> {
<a href="#l2067" class="line" id="l2067">   2067</a> 	Client *c;
<a href="#l2068" class="line" id="l2068">   2068</a> 	Monitor *m;
<a href="#l2069" class="line" id="l2069">   2069</a> 
<a href="#l2070" class="line" id="l2070">   2070</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l2071" class="line" id="l2071">   2071</a> 		for (c = m-&gt;clients; c; c = c-&gt;next)
<a href="#l2072" class="line" id="l2072">   2072</a> 			if (c-&gt;win == w)
<a href="#l2073" class="line" id="l2073">   2073</a> 				return c;
<a href="#l2074" class="line" id="l2074">   2074</a> 	return NULL;
<a href="#l2075" class="line" id="l2075">   2075</a> }
<a href="#l2076" class="line" id="l2076">   2076</a> 
<a href="#l2077" class="line" id="l2077">   2077</a> Monitor *
<a href="#l2078" class="line" id="l2078">   2078</a> wintomon(Window w)
<a href="#l2079" class="line" id="l2079">   2079</a> {
<a href="#l2080" class="line" id="l2080">   2080</a> 	int x, y;
<a href="#l2081" class="line" id="l2081">   2081</a> 	Client *c;
<a href="#l2082" class="line" id="l2082">   2082</a> 	Monitor *m;
<a href="#l2083" class="line" id="l2083">   2083</a> 
<a href="#l2084" class="line" id="l2084">   2084</a> 	if (w == root &amp;&amp; getrootptr(&amp;x, &amp;y))
<a href="#l2085" class="line" id="l2085">   2085</a> 		return recttomon(x, y, 1, 1);
<a href="#l2086" class="line" id="l2086">   2086</a> 	for (m = mons; m; m = m-&gt;next)
<a href="#l2087" class="line" id="l2087">   2087</a> 		if (w == m-&gt;barwin)
<a href="#l2088" class="line" id="l2088">   2088</a> 			return m;
<a href="#l2089" class="line" id="l2089">   2089</a> 	if ((c = wintoclient(w)))
<a href="#l2090" class="line" id="l2090">   2090</a> 		return c-&gt;mon;
<a href="#l2091" class="line" id="l2091">   2091</a> 	return selmon;
<a href="#l2092" class="line" id="l2092">   2092</a> }
<a href="#l2093" class="line" id="l2093">   2093</a> 
<a href="#l2094" class="line" id="l2094">   2094</a> /* There&#39;s no way to check accesses to destroyed windows, thus those cases are
<a href="#l2095" class="line" id="l2095">   2095</a>  * ignored (especially on UnmapNotify&#39;s). Other types of errors call Xlibs
<a href="#l2096" class="line" id="l2096">   2096</a>  * default error handler, which may call exit. */
<a href="#l2097" class="line" id="l2097">   2097</a> int
<a href="#l2098" class="line" id="l2098">   2098</a> xerror(Display *dpy, XErrorEvent *ee)
<a href="#l2099" class="line" id="l2099">   2099</a> {
<a href="#l2100" class="line" id="l2100">   2100</a> 	if (ee-&gt;error_code == BadWindow
<a href="#l2101" class="line" id="l2101">   2101</a> 	|| (ee-&gt;request_code == X_SetInputFocus &amp;&amp; ee-&gt;error_code == BadMatch)
<a href="#l2102" class="line" id="l2102">   2102</a> 	|| (ee-&gt;request_code == X_PolyText8 &amp;&amp; ee-&gt;error_code == BadDrawable)
<a href="#l2103" class="line" id="l2103">   2103</a> 	|| (ee-&gt;request_code == X_PolyFillRectangle &amp;&amp; ee-&gt;error_code == BadDrawable)
<a href="#l2104" class="line" id="l2104">   2104</a> 	|| (ee-&gt;request_code == X_PolySegment &amp;&amp; ee-&gt;error_code == BadDrawable)
<a href="#l2105" class="line" id="l2105">   2105</a> 	|| (ee-&gt;request_code == X_ConfigureWindow &amp;&amp; ee-&gt;error_code == BadMatch)
<a href="#l2106" class="line" id="l2106">   2106</a> 	|| (ee-&gt;request_code == X_GrabButton &amp;&amp; ee-&gt;error_code == BadAccess)
<a href="#l2107" class="line" id="l2107">   2107</a> 	|| (ee-&gt;request_code == X_GrabKey &amp;&amp; ee-&gt;error_code == BadAccess)
<a href="#l2108" class="line" id="l2108">   2108</a> 	|| (ee-&gt;request_code == X_CopyArea &amp;&amp; ee-&gt;error_code == BadDrawable))
<a href="#l2109" class="line" id="l2109">   2109</a> 		return 0;
<a href="#l2110" class="line" id="l2110">   2110</a> 	fprintf(stderr, &quot;dwm: fatal error: request code=%d, error code=%d\n&quot;,
<a href="#l2111" class="line" id="l2111">   2111</a> 		ee-&gt;request_code, ee-&gt;error_code);
<a href="#l2112" class="line" id="l2112">   2112</a> 	return xerrorxlib(dpy, ee); /* may call exit */
<a href="#l2113" class="line" id="l2113">   2113</a> }
<a href="#l2114" class="line" id="l2114">   2114</a> 
<a href="#l2115" class="line" id="l2115">   2115</a> int
<a href="#l2116" class="line" id="l2116">   2116</a> xerrordummy(Display *dpy, XErrorEvent *ee)
<a href="#l2117" class="line" id="l2117">   2117</a> {
<a href="#l2118" class="line" id="l2118">   2118</a> 	return 0;
<a href="#l2119" class="line" id="l2119">   2119</a> }
<a href="#l2120" class="line" id="l2120">   2120</a> 
<a href="#l2121" class="line" id="l2121">   2121</a> /* Startup Error handler to check if another window manager
<a href="#l2122" class="line" id="l2122">   2122</a>  * is already running. */
<a href="#l2123" class="line" id="l2123">   2123</a> int
<a href="#l2124" class="line" id="l2124">   2124</a> xerrorstart(Display *dpy, XErrorEvent *ee)
<a href="#l2125" class="line" id="l2125">   2125</a> {
<a href="#l2126" class="line" id="l2126">   2126</a> 	die(&quot;dwm: another window manager is already running&quot;);
<a href="#l2127" class="line" id="l2127">   2127</a> 	return -1;
<a href="#l2128" class="line" id="l2128">   2128</a> }
<a href="#l2129" class="line" id="l2129">   2129</a> 
<a href="#l2130" class="line" id="l2130">   2130</a> void
<a href="#l2131" class="line" id="l2131">   2131</a> zoom(const Arg *arg)
<a href="#l2132" class="line" id="l2132">   2132</a> {
<a href="#l2133" class="line" id="l2133">   2133</a> 	Client *c = selmon-&gt;sel;
<a href="#l2134" class="line" id="l2134">   2134</a> 
<a href="#l2135" class="line" id="l2135">   2135</a> 	if (!selmon-&gt;lt[selmon-&gt;sellt]-&gt;arrange || !c || c-&gt;isfloating)
<a href="#l2136" class="line" id="l2136">   2136</a> 		return;
<a href="#l2137" class="line" id="l2137">   2137</a> 	if (c == nexttiled(selmon-&gt;clients) &amp;&amp; !(c = nexttiled(c-&gt;next)))
<a href="#l2138" class="line" id="l2138">   2138</a> 		return;
<a href="#l2139" class="line" id="l2139">   2139</a> 	pop(c);
<a href="#l2140" class="line" id="l2140">   2140</a> }
<a href="#l2141" class="line" id="l2141">   2141</a> 
<a href="#l2142" class="line" id="l2142">   2142</a> int
<a href="#l2143" class="line" id="l2143">   2143</a> main(int argc, char *argv[])
<a href="#l2144" class="line" id="l2144">   2144</a> {
<a href="#l2145" class="line" id="l2145">   2145</a> 	if (argc == 2 &amp;&amp; !strcmp(&quot;-v&quot;, argv[1]))
<a href="#l2146" class="line" id="l2146">   2146</a> 		die(&quot;dwm-&quot;VERSION);
<a href="#l2147" class="line" id="l2147">   2147</a> 	else if (argc != 1)
<a href="#l2148" class="line" id="l2148">   2148</a> 		die(&quot;usage: dwm [-v]&quot;);
<a href="#l2149" class="line" id="l2149">   2149</a> 	if (!setlocale(LC_CTYPE, &quot;&quot;) || !XSupportsLocale())
<a href="#l2150" class="line" id="l2150">   2150</a> 		fputs(&quot;warning: no locale support\n&quot;, stderr);
<a href="#l2151" class="line" id="l2151">   2151</a> 	if (!(dpy = XOpenDisplay(NULL)))
<a href="#l2152" class="line" id="l2152">   2152</a> 		die(&quot;dwm: cannot open display&quot;);
<a href="#l2153" class="line" id="l2153">   2153</a> 	checkotherwm();
<a href="#l2154" class="line" id="l2154">   2154</a> 	setup();
<a href="#l2155" class="line" id="l2155">   2155</a> #ifdef __OpenBSD__
<a href="#l2156" class="line" id="l2156">   2156</a> 	if (pledge(&quot;stdio rpath proc exec&quot;, NULL) == -1)
<a href="#l2157" class="line" id="l2157">   2157</a> 		die(&quot;pledge&quot;);
<a href="#l2158" class="line" id="l2158">   2158</a> #endif /* __OpenBSD__ */
<a href="#l2159" class="line" id="l2159">   2159</a> 	scan();
<a href="#l2160" class="line" id="l2160">   2160</a> 	run();
<a href="#l2161" class="line" id="l2161">   2161</a> 	cleanup();
<a href="#l2162" class="line" id="l2162">   2162</a> 	XCloseDisplay(dpy);
<a href="#l2163" class="line" id="l2163">   2163</a> 	return EXIT_SUCCESS;
<a href="#l2164" class="line" id="l2164">   2164</a> }
</pre>
</div>
</body>
</html>
