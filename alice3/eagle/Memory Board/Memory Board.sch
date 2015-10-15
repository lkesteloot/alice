<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.4.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="9" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="con-subd">
<description>&lt;b&gt;SUB-D Connectors&lt;/b&gt;&lt;p&gt;
Harting&lt;br&gt;
NorComp&lt;br&gt;
&lt;p&gt;
PREFIX :&lt;br&gt;
H = High density&lt;br&gt;
F = Female&lt;br&gt;
M = Male&lt;p&gt;
NUMBER: Number of pins&lt;p&gt;
SUFFIX :&lt;br&gt;
H = Horizontal&lt;br&gt;
V = Vertical &lt;br&gt;
P = Shield pin on housing&lt;br&gt;
B = No mounting holes&lt;br&gt;
S = Pins individually placeable (Single)&lt;br&gt;
D = Direct mounting &lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="M25D">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="-19.812" y1="6.745" x2="-19.304" y2="7.253" width="0.1524" layer="21" curve="-90"/>
<wire x1="19.304" y1="7.253" x2="19.812" y2="6.745" width="0.1524" layer="21" curve="-90"/>
<wire x1="19.812" y1="2.046" x2="19.812" y2="6.745" width="0.1524" layer="21"/>
<wire x1="19.812" y1="2.046" x2="20.447" y2="1.411" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.812" y1="2.046" x2="-19.812" y2="6.745" width="0.1524" layer="21"/>
<wire x1="-20.447" y1="1.411" x2="-19.812" y2="2.046" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.304" y1="7.253" x2="19.304" y2="7.253" width="0.1524" layer="21"/>
<wire x1="-24.6888" y1="-2.7432" x2="-23.3172" y2="-4.1148" width="0.254" layer="48"/>
<wire x1="-23.3172" y1="-4.1148" x2="-21.9456" y2="-2.7432" width="0.254" layer="48"/>
<wire x1="-21.9456" y1="-2.7432" x2="-20.574" y2="-4.1148" width="0.254" layer="48"/>
<wire x1="-20.574" y1="-4.1148" x2="-19.2024" y2="-2.7432" width="0.254" layer="48"/>
<wire x1="19.2024" y1="-2.7432" x2="20.574" y2="-4.1148" width="0.254" layer="48"/>
<wire x1="20.574" y1="-4.1148" x2="21.9456" y2="-2.7432" width="0.254" layer="48"/>
<wire x1="21.9456" y1="-2.7432" x2="23.3172" y2="-4.1148" width="0.254" layer="48"/>
<wire x1="23.3172" y1="-4.1148" x2="24.6888" y2="-2.7432" width="0.254" layer="48"/>
<wire x1="5.08" y1="3.669" x2="2.794" y2="3.669" width="0.1524" layer="21"/>
<wire x1="2.794" y1="3.669" x2="2.794" y2="5.955" width="0.1524" layer="21"/>
<wire x1="5.08" y1="5.955" x2="5.08" y2="3.669" width="0.1524" layer="21"/>
<wire x1="1.143" y1="3.669" x2="-1.143" y2="3.669" width="0.1524" layer="21"/>
<wire x1="-1.143" y1="3.669" x2="-1.143" y2="5.955" width="0.1524" layer="21"/>
<wire x1="1.143" y1="5.955" x2="1.143" y2="3.669" width="0.1524" layer="21"/>
<wire x1="-2.794" y1="3.669" x2="-5.08" y2="3.669" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="3.669" x2="-5.08" y2="5.955" width="0.1524" layer="21"/>
<wire x1="-2.794" y1="5.955" x2="-2.794" y2="3.669" width="0.1524" layer="21"/>
<wire x1="-18.6436" y1="0.508" x2="-18.6436" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-18.6436" y1="0.508" x2="-18.5166" y2="0.508" width="0.1524" layer="21"/>
<wire x1="-18.5166" y1="-1.905" x2="-17.8816" y2="-2.54" width="0.1524" layer="21" curve="90"/>
<wire x1="-18.5166" y1="-1.905" x2="-18.5166" y2="0.508" width="0.1524" layer="21"/>
<wire x1="-17.901" y1="-2.54" x2="17.901" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="18.6436" y1="0.508" x2="18.6436" y2="1.016" width="0.1524" layer="21"/>
<wire x1="18.5166" y1="-1.905" x2="18.5166" y2="0.508" width="0.1524" layer="21"/>
<wire x1="18.5166" y1="0.508" x2="18.6436" y2="0.508" width="0.1524" layer="21"/>
<wire x1="17.8816" y1="-2.54" x2="18.5166" y2="-1.905" width="0.1524" layer="21" curve="90"/>
<smd name="1" x="16.4594" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="2" x="13.716" y="-6.045" dx="1.7" dy="3.5" layer="1"/>
<smd name="3" x="10.9726" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="4" x="8.2298" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="5" x="5.4862" y="-6.0446" dx="1.7" dy="3.5" layer="1"/>
<smd name="6" x="2.7434" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="7" x="0" y="-6.045" dx="1.7" dy="3.5" layer="1"/>
<smd name="8" x="-2.7434" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="9" x="-5.4862" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="10" x="-8.2298" y="-6.0446" dx="1.7" dy="3.5" layer="1"/>
<smd name="11" x="-10.9726" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="12" x="-13.716" y="-6.045" dx="1.7" dy="3.5" layer="1"/>
<smd name="13" x="-16.4594" y="-6.0448" dx="1.7" dy="3.5" layer="1"/>
<smd name="14" x="15.0876" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="15" x="12.3444" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="16" x="9.6012" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="17" x="6.858" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="18" x="4.1148" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="19" x="1.3716" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="20" x="-1.3716" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="21" x="-4.1148" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="22" x="-6.858" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="23" x="-9.6012" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="24" x="-12.3444" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<smd name="25" x="-15.0876" y="-6.0452" dx="1.7" dy="3.5" layer="16"/>
<text x="-10.2616" y="-10.16" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-0.1016" y="-10.16" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="16.0256" y="-2.1786" size="1.27" layer="21" ratio="10">1</text>
<text x="19.6596" y="-2.54" size="1.27" layer="48">Board</text>
<text x="-14.6606" y="-0.825" size="1.27" layer="22" ratio="10" rot="MR0">25</text>
<text x="-16.8606" y="-2.1786" size="1.27" layer="21" ratio="10">13</text>
<text x="15.6256" y="-0.825" size="1.27" layer="22" ratio="10" rot="MR0">14</text>
<text x="-17.2212" y="4.5974" size="1.27" layer="51" ratio="10">M25D</text>
<text x="-24.7396" y="-2.54" size="1.27" layer="48">Board </text>
<rectangle x1="14.4876" y1="-6.56" x2="15.6876" y2="-4.06" layer="52"/>
<rectangle x1="15.8592" y1="-6.56" x2="17.0592" y2="-4.06" layer="51"/>
<rectangle x1="11.7444" y1="-6.56" x2="12.9444" y2="-4.06" layer="52"/>
<rectangle x1="13.116" y1="-6.56" x2="14.316" y2="-4.06" layer="51"/>
<rectangle x1="9.0012" y1="-6.56" x2="10.2012" y2="-4.06" layer="52"/>
<rectangle x1="10.3728" y1="-6.56" x2="11.5728" y2="-4.06" layer="51"/>
<rectangle x1="6.258" y1="-6.56" x2="7.458" y2="-4.06" layer="52"/>
<rectangle x1="7.6296" y1="-6.56" x2="8.8296" y2="-4.06" layer="51"/>
<rectangle x1="3.5148" y1="-6.56" x2="4.7148" y2="-4.06" layer="52"/>
<rectangle x1="4.8864" y1="-6.56" x2="6.0864" y2="-4.06" layer="51"/>
<rectangle x1="0.7716" y1="-6.56" x2="1.9716" y2="-4.06" layer="52"/>
<rectangle x1="2.1432" y1="-6.56" x2="3.3432" y2="-4.06" layer="51"/>
<rectangle x1="-1.9716" y1="-6.56" x2="-0.7716" y2="-4.06" layer="52"/>
<rectangle x1="-0.6" y1="-6.56" x2="0.6" y2="-4.06" layer="51"/>
<rectangle x1="-3.3432" y1="-6.56" x2="-2.1432" y2="-4.06" layer="51"/>
<rectangle x1="-4.7148" y1="-6.56" x2="-3.5148" y2="-4.06" layer="52"/>
<rectangle x1="-7.458" y1="-6.56" x2="-6.258" y2="-4.06" layer="52"/>
<rectangle x1="-6.0864" y1="-6.56" x2="-4.8864" y2="-4.06" layer="51"/>
<rectangle x1="-10.2012" y1="-6.56" x2="-9.0012" y2="-4.06" layer="52"/>
<rectangle x1="-8.8296" y1="-6.56" x2="-7.6296" y2="-4.06" layer="51"/>
<rectangle x1="-12.9444" y1="-6.56" x2="-11.7444" y2="-4.06" layer="52"/>
<rectangle x1="-11.5728" y1="-6.56" x2="-10.3728" y2="-4.06" layer="51"/>
<rectangle x1="-15.6876" y1="-6.56" x2="-14.4876" y2="-4.06" layer="52"/>
<rectangle x1="-14.316" y1="-6.56" x2="-13.116" y2="-4.06" layer="51"/>
<rectangle x1="-17.0592" y1="-6.56" x2="-15.8592" y2="-4.06" layer="51"/>
<rectangle x1="-26.5" y1="0.9" x2="26.5" y2="1.4" layer="21"/>
<rectangle x1="-17.5" y1="-4.1" x2="17.5" y2="-2.5" layer="21"/>
</package>
<package name="M25H">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="19.304" y1="-17.526" x2="19.05" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.304" y1="-17.526" x2="19.812" y2="-17.018" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.812" y1="-17.018" x2="-19.304" y2="-17.526" width="0.1524" layer="21" curve="90"/>
<wire x1="19.431" y1="-11.684" x2="-19.431" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="26.543" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-10.668" x2="26.543" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-11.176" x2="26.543" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-11.684" x2="-26.543" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-11.176" x2="-26.543" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-10.668" x2="-26.543" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-7.62" x2="-26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-10.668" x2="21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-10.668" x2="21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="20.701" y1="-7.62" x2="21.336" y2="-8.255" width="0.1524" layer="21" curve="-90"/>
<wire x1="21.463" y1="-7.62" x2="20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="20.574" y1="-7.62" x2="20.32" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="21.336" y1="-8.255" x2="21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-7.62" x2="20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-7.62" x2="-20.32" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-6.858" x2="-20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-7.62" x2="-20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-7.62" x2="-20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="-7.62" x2="-21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-10.668" x2="-21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-8.255" x2="-21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-10.668" x2="-21.463" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-8.255" x2="-20.701" y2="-7.62" width="0.1524" layer="21" curve="-90"/>
<wire x1="25.908" y1="3.175" x2="26.543" y2="2.54" width="0.1524" layer="21"/>
<wire x1="26.543" y1="2.54" x2="26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="21.082" y1="3.175" x2="25.908" y2="3.175" width="0.1524" layer="21"/>
<wire x1="21.082" y1="3.175" x2="20.574" y2="2.667" width="0.1524" layer="21"/>
<wire x1="20.574" y1="2.667" x2="20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="2.667" x2="-20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="2.667" x2="-21.082" y2="3.175" width="0.1524" layer="21"/>
<wire x1="-21.082" y1="3.175" x2="-26.035" y2="3.175" width="0.1524" layer="21"/>
<wire x1="-26.035" y1="3.175" x2="-26.543" y2="2.667" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="2.667" x2="-26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="-2.7686" y1="1.143" x2="-2.7686" y2="0.127" width="0.8128" layer="51"/>
<wire x1="0" y1="1.143" x2="0" y2="0.127" width="0.8128" layer="51"/>
<wire x1="2.7686" y1="1.143" x2="2.7686" y2="0.127" width="0.8128" layer="51"/>
<wire x1="1.3716" y1="-1.397" x2="1.3716" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-1.3716" y1="-1.397" x2="-1.3716" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-4.1402" y1="-1.397" x2="-4.1402" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-5.5118" y1="1.143" x2="-5.5118" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-6.9088" y1="-1.397" x2="-6.9088" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-8.2804" y1="1.143" x2="-8.2804" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-21.463" y1="-7.62" x2="-26.543" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-19.431" y1="-11.684" x2="-26.543" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-19.812" y1="-12.319" x2="-19.812" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="-20.447" y1="-11.684" x2="-19.812" y2="-12.319" width="0.1524" layer="21" curve="-90"/>
<wire x1="26.543" y1="-11.684" x2="19.431" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="19.812" y1="-12.319" x2="19.812" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="19.812" y1="-12.319" x2="20.447" y2="-11.684" width="0.1524" layer="21" curve="-90"/>
<wire x1="-9.652" y1="-1.3716" x2="-9.652" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-11.049" y1="1.143" x2="-11.049" y2="0.127" width="0.8128" layer="51"/>
<wire x1="4.1402" y1="-1.397" x2="4.1402" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="5.5118" y1="1.143" x2="5.5118" y2="0.127" width="0.8128" layer="51"/>
<wire x1="6.9088" y1="-1.397" x2="6.9088" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="8.2804" y1="1.143" x2="8.2804" y2="0.127" width="0.8128" layer="51"/>
<wire x1="9.652" y1="-1.397" x2="9.652" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="11.049" y1="1.143" x2="11.049" y2="0.127" width="0.8128" layer="51"/>
<wire x1="13.7922" y1="1.143" x2="13.7922" y2="0.127" width="0.8128" layer="51"/>
<wire x1="12.4206" y1="-1.397" x2="12.4206" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="16.5608" y1="1.143" x2="16.5608" y2="0.127" width="0.8128" layer="51"/>
<wire x1="15.1892" y1="-1.397" x2="15.1892" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-13.7922" y1="1.143" x2="-13.7922" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-12.4206" y1="-1.3716" x2="-12.4206" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-15.1892" y1="-1.3716" x2="-15.1892" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-16.5608" y1="1.143" x2="-16.5608" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-19.05" y1="-17.526" x2="-19.304" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.05" y1="-17.526" x2="-19.05" y2="-17.526" width="0.1524" layer="21"/>
<circle x="23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<circle x="23.5204" y="0" radius="2.667" width="0" layer="42"/>
<circle x="23.5204" y="0" radius="2.667" width="0" layer="43"/>
<circle x="-23.5204" y="0" radius="2.667" width="0" layer="42"/>
<circle x="-23.5204" y="0" radius="2.667" width="0" layer="43"/>
<circle x="-23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<pad name="13" x="16.5608" y="1.27" drill="1.016" shape="octagon"/>
<pad name="12" x="13.7922" y="1.27" drill="1.016" shape="octagon"/>
<pad name="11" x="11.049" y="1.27" drill="1.016" shape="octagon"/>
<pad name="10" x="8.2804" y="1.27" drill="1.016" shape="octagon"/>
<pad name="9" x="5.5118" y="1.27" drill="1.016" shape="octagon"/>
<pad name="8" x="2.7686" y="1.27" drill="1.016" shape="octagon"/>
<pad name="7" x="0" y="1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="-2.7686" y="1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="-5.5118" y="1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="-8.2804" y="1.27" drill="1.016" shape="octagon"/>
<pad name="3" x="-11.049" y="1.27" drill="1.016" shape="octagon"/>
<pad name="2" x="-13.7922" y="1.27" drill="1.016" shape="octagon"/>
<pad name="1" x="-16.5608" y="1.27" drill="1.016" shape="octagon"/>
<pad name="25" x="15.1892" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="24" x="12.4206" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="23" x="9.652" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="22" x="6.9088" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="21" x="4.1402" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="20" x="1.3716" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="19" x="-1.3716" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="18" x="-4.1402" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="17" x="-6.9088" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="16" x="-9.652" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="15" x="-12.4206" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="14" x="-15.1892" y="-1.27" drill="1.016" shape="octagon"/>
<text x="-26.416" y="4.445" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-17.78" y="-20.447" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-18.669" y="0.635" size="1.27" layer="21" ratio="10">1</text>
<text x="17.907" y="0.635" size="1.27" layer="21" ratio="10">13</text>
<text x="-19.812" y="-2.032" size="1.27" layer="21" ratio="10">14</text>
<text x="17.653" y="-1.905" size="1.27" layer="21" ratio="10">25</text>
<text x="-24.638" y="-6.858" size="1.27" layer="21" ratio="10" rot="R90">2,54</text>
<text x="-18.669" y="-9.779" size="1.27" layer="21" ratio="10">M25</text>
<rectangle x1="-26.543" y1="-11.684" x2="26.543" y2="-11.176" layer="21"/>
<rectangle x1="-20.32" y1="-7.62" x2="20.32" y2="-6.858" layer="21"/>
<rectangle x1="-16.9672" y1="-6.858" x2="-16.1544" y2="0.381" layer="21"/>
<rectangle x1="-15.5956" y1="-6.858" x2="-14.7828" y2="-2.159" layer="21"/>
<rectangle x1="-14.1986" y1="-6.858" x2="-13.3858" y2="0.381" layer="21"/>
<rectangle x1="-12.827" y1="-6.858" x2="-12.0142" y2="-2.159" layer="21"/>
<rectangle x1="-11.4554" y1="-6.858" x2="-10.6426" y2="0.381" layer="21"/>
<rectangle x1="-10.0584" y1="-6.858" x2="-9.2456" y2="-2.159" layer="21"/>
<rectangle x1="-8.6868" y1="-6.858" x2="-7.874" y2="0.381" layer="21"/>
<rectangle x1="-7.3152" y1="-6.858" x2="-6.5024" y2="-2.159" layer="21"/>
<rectangle x1="-5.9182" y1="-6.858" x2="-5.1054" y2="0.381" layer="21"/>
<rectangle x1="-4.5466" y1="-6.858" x2="-3.7338" y2="-2.159" layer="21"/>
<rectangle x1="-3.175" y1="-6.858" x2="-2.3622" y2="0.381" layer="21"/>
<rectangle x1="-1.778" y1="-6.858" x2="-0.9652" y2="-2.159" layer="21"/>
<rectangle x1="-0.4064" y1="-6.858" x2="0.4064" y2="0.381" layer="21"/>
<rectangle x1="0.9652" y1="-6.858" x2="1.778" y2="-2.159" layer="21"/>
<rectangle x1="2.3622" y1="-6.858" x2="3.175" y2="0.381" layer="21"/>
<rectangle x1="3.7338" y1="-6.858" x2="4.5466" y2="-2.159" layer="21"/>
<rectangle x1="5.1054" y1="-6.858" x2="5.9182" y2="0.381" layer="21"/>
<rectangle x1="6.5024" y1="-6.858" x2="7.3152" y2="-2.159" layer="21"/>
<rectangle x1="7.874" y1="-6.858" x2="8.6868" y2="0.381" layer="21"/>
<rectangle x1="9.2456" y1="-6.858" x2="10.0584" y2="-2.159" layer="21"/>
<rectangle x1="10.6426" y1="-6.858" x2="11.4554" y2="0.381" layer="21"/>
<rectangle x1="12.0142" y1="-6.858" x2="12.827" y2="-2.159" layer="21"/>
<rectangle x1="13.3858" y1="-6.858" x2="14.1986" y2="0.381" layer="21"/>
<rectangle x1="14.7828" y1="-6.858" x2="15.5956" y2="-2.159" layer="21"/>
<rectangle x1="16.1544" y1="-6.858" x2="16.9672" y2="0.381" layer="21"/>
<hole x="23.5204" y="0" drill="3.302"/>
<hole x="-23.5204" y="0" drill="3.302"/>
</package>
<package name="M25HP">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="19.304" y1="-17.526" x2="19.05" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.304" y1="-17.526" x2="19.812" y2="-17.018" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.812" y1="-17.018" x2="-19.304" y2="-17.526" width="0.1524" layer="21" curve="90"/>
<wire x1="19.431" y1="-11.684" x2="19.431" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="19.431" y1="-11.684" x2="-19.431" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-19.431" y1="-11.684" x2="-19.431" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="26.543" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.62" x2="26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-10.668" x2="24.003" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-10.668" x2="26.543" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-11.176" x2="26.543" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-11.684" x2="-26.543" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-11.176" x2="-26.543" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-10.668" x2="-26.543" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-7.62" x2="-26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="24.003" y1="-10.414" x2="21.463" y2="-10.414" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-11.176" x2="24.003" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="24.003" y1="-11.176" x2="21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="24.003" y1="-10.668" x2="24.003" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="24.003" y1="-10.668" x2="24.003" y2="-10.414" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-10.414" x2="21.463" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-10.668" x2="21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="26.543" y1="-7.493" x2="21.463" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-7.493" x2="21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-10.668" x2="21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="21.336" y1="-10.668" x2="-21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="20.701" y1="-7.62" x2="21.336" y2="-8.255" width="0.1524" layer="21" curve="-90"/>
<wire x1="21.463" y1="-7.62" x2="20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="20.574" y1="-7.62" x2="20.32" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="21.336" y1="-8.255" x2="21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-7.62" x2="20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-7.62" x2="-20.32" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="20.32" y1="-6.858" x2="-20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-7.62" x2="-20.32" y2="-6.858" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-7.62" x2="-20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="-7.62" x2="-21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-7.493" x2="-26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="-24.003" y1="-10.668" x2="-24.003" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-24.003" y1="-10.668" x2="-26.543" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-24.003" y1="-11.176" x2="-26.543" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-10.414" x2="-24.003" y2="-10.414" width="0.1524" layer="21"/>
<wire x1="-24.003" y1="-10.668" x2="-24.003" y2="-10.414" width="0.1524" layer="21"/>
<wire x1="21.463" y1="-11.176" x2="-21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-11.176" x2="-24.003" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-10.414" x2="-21.463" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-10.668" x2="-21.463" y2="-11.176" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-8.255" x2="-21.336" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-10.668" x2="-21.463" y2="-10.668" width="0.1524" layer="21"/>
<wire x1="-21.336" y1="-8.255" x2="-20.701" y2="-7.62" width="0.1524" layer="21" curve="-90"/>
<wire x1="25.908" y1="3.175" x2="26.543" y2="2.54" width="0.1524" layer="21"/>
<wire x1="26.543" y1="2.54" x2="26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="21.082" y1="3.175" x2="25.908" y2="3.175" width="0.1524" layer="21"/>
<wire x1="21.082" y1="3.175" x2="20.574" y2="2.667" width="0.1524" layer="21"/>
<wire x1="20.574" y1="2.667" x2="20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="2.667" x2="-20.574" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-20.574" y1="2.667" x2="-21.082" y2="3.175" width="0.1524" layer="21"/>
<wire x1="-21.082" y1="3.175" x2="-26.035" y2="3.175" width="0.1524" layer="21"/>
<wire x1="-26.035" y1="3.175" x2="-26.543" y2="2.667" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="2.667" x2="-26.543" y2="-7.493" width="0.1524" layer="21"/>
<wire x1="-2.7686" y1="1.143" x2="-2.7686" y2="0.127" width="0.8128" layer="51"/>
<wire x1="0" y1="1.143" x2="0" y2="0.127" width="0.8128" layer="51"/>
<wire x1="2.7686" y1="1.143" x2="2.7686" y2="0.127" width="0.8128" layer="51"/>
<wire x1="1.3716" y1="-1.397" x2="1.3716" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-1.3716" y1="-1.397" x2="-1.3716" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-4.1402" y1="-1.397" x2="-4.1402" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-5.5118" y1="1.143" x2="-5.5118" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-6.9088" y1="-1.397" x2="-6.9088" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-8.2804" y1="1.143" x2="-8.2804" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-5.08" y1="-13.97" x2="-2.794" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-2.794" y1="-13.97" x2="-2.794" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-2.794" y1="-16.256" x2="-3.175" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-16.256" x2="-3.175" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-14.351" x2="-4.699" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-4.699" y1="-14.351" x2="-4.699" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-4.699" y1="-16.256" x2="-5.08" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-16.256" x2="-5.08" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-1.143" y1="-13.97" x2="1.143" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="1.143" y1="-13.97" x2="1.143" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="1.143" y1="-16.256" x2="0.762" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="0.762" y1="-16.256" x2="0.762" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="0.762" y1="-14.351" x2="-0.762" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-0.762" y1="-14.351" x2="-0.762" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-0.762" y1="-16.256" x2="-1.143" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-1.143" y1="-16.256" x2="-1.143" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="2.794" y1="-13.97" x2="5.08" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-13.97" x2="5.08" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-16.256" x2="4.699" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="4.699" y1="-16.256" x2="4.699" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="4.699" y1="-14.351" x2="3.175" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-14.351" x2="3.175" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-16.256" x2="2.794" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="2.794" y1="-16.256" x2="2.794" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-7.493" x2="-21.463" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-21.463" y1="-7.62" x2="-26.543" y2="-7.62" width="0.1524" layer="21"/>
<wire x1="-19.431" y1="-11.684" x2="-26.543" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-19.812" y1="-12.319" x2="-19.812" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="-20.447" y1="-11.684" x2="-19.812" y2="-12.319" width="0.1524" layer="21" curve="-90"/>
<wire x1="26.543" y1="-11.684" x2="19.431" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="19.812" y1="-12.319" x2="19.812" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="19.812" y1="-12.319" x2="20.447" y2="-11.684" width="0.1524" layer="21" curve="-90"/>
<wire x1="-9.652" y1="-1.3716" x2="-9.652" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-11.049" y1="1.143" x2="-11.049" y2="0.127" width="0.8128" layer="51"/>
<wire x1="4.1402" y1="-1.397" x2="4.1402" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="5.5118" y1="1.143" x2="5.5118" y2="0.127" width="0.8128" layer="51"/>
<wire x1="6.9088" y1="-1.397" x2="6.9088" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="8.2804" y1="1.143" x2="8.2804" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-9.017" y1="-13.97" x2="-6.731" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-6.731" y1="-13.97" x2="-6.731" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-6.731" y1="-16.256" x2="-7.112" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-7.112" y1="-16.256" x2="-7.112" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-7.112" y1="-14.351" x2="-8.636" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-8.636" y1="-14.351" x2="-8.636" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-8.636" y1="-16.256" x2="-9.017" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-9.017" y1="-16.256" x2="-9.017" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="6.731" y1="-13.97" x2="9.017" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="9.017" y1="-13.97" x2="9.017" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="9.017" y1="-16.256" x2="8.636" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="8.636" y1="-16.256" x2="8.636" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="8.636" y1="-14.351" x2="7.112" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="7.112" y1="-14.351" x2="7.112" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="7.112" y1="-16.256" x2="6.731" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="6.731" y1="-16.256" x2="6.731" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="9.652" y1="-1.397" x2="9.652" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="11.049" y1="1.143" x2="11.049" y2="0.127" width="0.8128" layer="51"/>
<wire x1="13.7922" y1="1.143" x2="13.7922" y2="0.127" width="0.8128" layer="51"/>
<wire x1="12.4206" y1="-1.397" x2="12.4206" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="16.5608" y1="1.143" x2="16.5608" y2="0.127" width="0.8128" layer="51"/>
<wire x1="15.1892" y1="-1.397" x2="15.1892" y2="-2.413" width="0.8128" layer="51"/>
<wire x1="-13.7922" y1="1.143" x2="-13.7922" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-12.4206" y1="-1.3716" x2="-12.4206" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-15.1892" y1="-1.3716" x2="-15.1892" y2="-2.3876" width="0.8128" layer="51"/>
<wire x1="-16.5608" y1="1.143" x2="-16.5608" y2="0.127" width="0.8128" layer="51"/>
<wire x1="-12.954" y1="-13.97" x2="-10.668" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-10.668" y1="-13.97" x2="-10.668" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-10.668" y1="-16.256" x2="-11.049" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-11.049" y1="-16.256" x2="-11.049" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-11.049" y1="-14.351" x2="-12.573" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-12.573" y1="-14.351" x2="-12.573" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-12.573" y1="-16.256" x2="-12.954" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-12.954" y1="-16.256" x2="-12.954" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="10.668" y1="-13.97" x2="12.954" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="12.954" y1="-13.97" x2="12.954" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="12.954" y1="-16.256" x2="12.573" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="12.573" y1="-16.256" x2="12.573" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="12.573" y1="-14.351" x2="11.049" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="11.049" y1="-14.351" x2="11.049" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="11.049" y1="-16.256" x2="10.668" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="10.668" y1="-16.256" x2="10.668" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-19.05" y1="-12.192" x2="-19.05" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="-19.05" y1="-17.526" x2="-19.304" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.05" y1="-12.192" x2="19.05" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.05" y1="-17.526" x2="-19.05" y2="-17.526" width="0.1524" layer="21"/>
<circle x="23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<circle x="-23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<pad name="13" x="16.5608" y="1.27" drill="1.016" shape="octagon"/>
<pad name="12" x="13.7922" y="1.27" drill="1.016" shape="octagon"/>
<pad name="11" x="11.049" y="1.27" drill="1.016" shape="octagon"/>
<pad name="10" x="8.2804" y="1.27" drill="1.016" shape="octagon"/>
<pad name="9" x="5.5118" y="1.27" drill="1.016" shape="octagon"/>
<pad name="8" x="2.7686" y="1.27" drill="1.016" shape="octagon"/>
<pad name="7" x="0" y="1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="-2.7686" y="1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="-5.5118" y="1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="-8.2804" y="1.27" drill="1.016" shape="octagon"/>
<pad name="3" x="-11.049" y="1.27" drill="1.016" shape="octagon"/>
<pad name="2" x="-13.7922" y="1.27" drill="1.016" shape="octagon"/>
<pad name="1" x="-16.5608" y="1.27" drill="1.016" shape="octagon"/>
<pad name="25" x="15.1892" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="24" x="12.4206" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="23" x="9.652" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="22" x="6.9088" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="21" x="4.1402" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="20" x="1.3716" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="19" x="-1.3716" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="18" x="-4.1402" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="17" x="-6.9088" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="16" x="-9.652" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="15" x="-12.4206" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="14" x="-15.1892" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="G1" x="-23.5204" y="0" drill="3.302" diameter="5.08"/>
<pad name="G2" x="23.5204" y="0" drill="3.302" diameter="5.08"/>
<text x="-26.416" y="4.445" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-17.78" y="-20.447" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-18.669" y="0.635" size="1.27" layer="21" ratio="10">1</text>
<text x="17.907" y="0.635" size="1.27" layer="21" ratio="10">13</text>
<text x="-19.812" y="-2.032" size="1.27" layer="21" ratio="10">14</text>
<text x="17.653" y="-1.905" size="1.27" layer="21" ratio="10">25</text>
<text x="-24.638" y="-6.858" size="1.27" layer="21" ratio="10" rot="R90">2,54</text>
<text x="-18.669" y="-9.779" size="1.27" layer="21" ratio="10">M25</text>
<rectangle x1="20.701" y1="-10.668" x2="20.955" y2="-8.255" layer="21"/>
<rectangle x1="-20.955" y1="-10.668" x2="-20.701" y2="-8.255" layer="21"/>
<rectangle x1="23.0632" y1="-7.62" x2="24.0792" y2="-5.969" layer="21"/>
<rectangle x1="-24.0792" y1="-7.62" x2="-23.0632" y2="-5.969" layer="21"/>
<rectangle x1="-26.543" y1="-11.684" x2="26.543" y2="-11.176" layer="21"/>
<rectangle x1="-20.32" y1="-7.62" x2="20.32" y2="-6.858" layer="21"/>
<rectangle x1="-16.9672" y1="-6.858" x2="-16.1544" y2="0.381" layer="21"/>
<rectangle x1="-15.5956" y1="-6.858" x2="-14.7828" y2="-2.159" layer="21"/>
<rectangle x1="-14.1986" y1="-6.858" x2="-13.3858" y2="0.381" layer="21"/>
<rectangle x1="-12.827" y1="-6.858" x2="-12.0142" y2="-2.159" layer="21"/>
<rectangle x1="-11.4554" y1="-6.858" x2="-10.6426" y2="0.381" layer="21"/>
<rectangle x1="-10.0584" y1="-6.858" x2="-9.2456" y2="-2.159" layer="21"/>
<rectangle x1="-8.6868" y1="-6.858" x2="-7.874" y2="0.381" layer="21"/>
<rectangle x1="-7.3152" y1="-6.858" x2="-6.5024" y2="-2.159" layer="21"/>
<rectangle x1="-5.9182" y1="-6.858" x2="-5.1054" y2="0.381" layer="21"/>
<rectangle x1="-4.5466" y1="-6.858" x2="-3.7338" y2="-2.159" layer="21"/>
<rectangle x1="-3.175" y1="-6.858" x2="-2.3622" y2="0.381" layer="21"/>
<rectangle x1="-1.778" y1="-6.858" x2="-0.9652" y2="-2.159" layer="21"/>
<rectangle x1="-0.4064" y1="-6.858" x2="0.4064" y2="0.381" layer="21"/>
<rectangle x1="0.9652" y1="-6.858" x2="1.778" y2="-2.159" layer="21"/>
<rectangle x1="2.3622" y1="-6.858" x2="3.175" y2="0.381" layer="21"/>
<rectangle x1="3.7338" y1="-6.858" x2="4.5466" y2="-2.159" layer="21"/>
<rectangle x1="5.1054" y1="-6.858" x2="5.9182" y2="0.381" layer="21"/>
<rectangle x1="6.5024" y1="-6.858" x2="7.3152" y2="-2.159" layer="21"/>
<rectangle x1="7.874" y1="-6.858" x2="8.6868" y2="0.381" layer="21"/>
<rectangle x1="9.2456" y1="-6.858" x2="10.0584" y2="-2.159" layer="21"/>
<rectangle x1="10.6426" y1="-6.858" x2="11.4554" y2="0.381" layer="21"/>
<rectangle x1="12.0142" y1="-6.858" x2="12.827" y2="-2.159" layer="21"/>
<rectangle x1="13.3858" y1="-6.858" x2="14.1986" y2="0.381" layer="21"/>
<rectangle x1="14.7828" y1="-6.858" x2="15.5956" y2="-2.159" layer="21"/>
<rectangle x1="16.1544" y1="-6.858" x2="16.9672" y2="0.381" layer="21"/>
</package>
<package name="M25V">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="-18.5674" y1="-2.9464" x2="-19.3548" y2="2.3368" width="0.1524" layer="21"/>
<wire x1="17.272" y1="-3.937" x2="18.5807" y2="-2.905" width="0.1524" layer="21" curve="76.489196"/>
<wire x1="18.034" y1="3.937" x2="19.3495" y2="2.3038" width="0.1524" layer="21" curve="-102.298925"/>
<wire x1="19.3548" y1="2.3114" x2="18.5674" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="-18.034" y1="3.937" x2="18.034" y2="3.937" width="0.1524" layer="21"/>
<wire x1="-19.3541" y1="2.3268" x2="-18.034" y2="3.937" width="0.1524" layer="21" curve="-101.30773"/>
<wire x1="-18.5749" y1="-2.9295" x2="-17.272" y2="-3.937" width="0.1524" layer="21" curve="75.428151"/>
<wire x1="-17.272" y1="-3.937" x2="17.272" y2="-3.937" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.223" x2="-23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.223" x2="-21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.096" x2="-21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.096" x2="-21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.223" x2="21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-25.908" y1="-6.223" x2="-23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-5.588" x2="-25.908" y2="-6.223" width="0.1524" layer="21" curve="90"/>
<wire x1="25.908" y1="-6.223" x2="26.543" y2="-5.588" width="0.1524" layer="21" curve="90"/>
<wire x1="25.908" y1="6.223" x2="23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="26.543" y1="5.588" x2="26.543" y2="-5.588" width="0.1524" layer="21"/>
<wire x1="25.908" y1="6.223" x2="26.543" y2="5.588" width="0.1524" layer="21" curve="-90"/>
<wire x1="-26.543" y1="5.588" x2="-26.543" y2="-5.588" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="5.588" x2="-25.908" y2="6.223" width="0.1524" layer="21" curve="-90"/>
<wire x1="21.209" y1="-6.223" x2="21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.223" x2="23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.096" x2="23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.096" x2="23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.223" x2="25.908" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.223" x2="21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.223" x2="-21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.096" x2="23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.096" x2="23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.223" x2="21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.223" x2="-23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.223" x2="-25.908" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.096" x2="-21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.096" x2="-21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.223" x2="-23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-18.034" y1="4.318" x2="18.034" y2="4.318" width="0.1524" layer="21"/>
<wire x1="19.7358" y1="2.3114" x2="18.9738" y2="-2.8702" width="0.1524" layer="21"/>
<wire x1="18.034" y1="4.318" x2="19.728" y2="2.2521" width="0.1524" layer="21" curve="-101.297755"/>
<wire x1="-18.9484" y1="-2.9464" x2="-19.7358" y2="2.3368" width="0.1524" layer="21"/>
<wire x1="-19.7277" y1="2.2521" x2="-18.034" y2="4.318" width="0.1524" layer="21" curve="-101.307706"/>
<wire x1="-17.272" y1="-4.318" x2="17.272" y2="-4.318" width="0.1524" layer="21"/>
<wire x1="-18.984" y1="-2.8191" x2="-17.272" y2="-4.318" width="0.1524" layer="21" curve="82.402958"/>
<wire x1="17.272" y1="-4.318" x2="18.9963" y2="-2.6849" width="0.1524" layer="21" curve="86.865803"/>
<circle x="23.5204" y="0" radius="2.667" width="0" layer="42"/>
<circle x="23.5204" y="0" radius="2.667" width="0" layer="43"/>
<circle x="-23.5204" y="0" radius="2.667" width="0" layer="42"/>
<circle x="-23.5204" y="0" radius="2.667" width="0" layer="43"/>
<circle x="-23.5204" y="-0.0254" radius="1.651" width="0.1524" layer="21"/>
<circle x="23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<circle x="-2.7686" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-4.1402" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-5.5118" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="0" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-1.3716" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="1.3716" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="2.7686" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="5.5118" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="4.1402" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="6.9088" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="8.2804" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="9.652" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="11.049" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="12.4206" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="13.7922" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="16.5608" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="15.1892" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-8.2804" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-6.9088" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-9.652" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-11.049" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-13.7922" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-16.5608" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-12.4206" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-15.1892" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<pad name="13" x="16.5608" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="12" x="13.7922" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="11" x="11.049" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="10" x="8.2804" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="9" x="5.5118" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="8" x="2.7686" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="7" x="0" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="6" x="-2.7686" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="5" x="-5.5118" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="4" x="-8.2804" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="3" x="-11.049" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="2" x="-13.7922" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="1" x="-16.5608" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="25" x="15.1892" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="24" x="12.4206" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="23" x="9.652" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="22" x="6.9088" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="21" x="4.1402" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="20" x="1.3716" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="19" x="-1.3716" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="18" x="-4.1402" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="17" x="-6.9088" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="16" x="-9.652" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="15" x="-12.4206" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="14" x="-15.1892" y="-1.4224" drill="1.016" shape="octagon"/>
<text x="-26.289" y="6.985" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.54" y="6.985" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-16.764" y="2.413" size="0.9906" layer="21" ratio="12">1</text>
<text x="-14.097" y="2.413" size="0.9906" layer="21" ratio="12">2</text>
<text x="-11.43" y="2.413" size="0.9906" layer="21" ratio="12">3</text>
<text x="-8.763" y="2.413" size="0.9906" layer="21" ratio="12">4</text>
<text x="-5.969" y="2.413" size="0.9906" layer="21" ratio="12">5</text>
<text x="-3.175" y="2.413" size="0.9906" layer="21" ratio="12">6</text>
<text x="-0.381" y="2.413" size="0.9906" layer="21" ratio="12">7</text>
<text x="2.413" y="2.413" size="0.9906" layer="21" ratio="12">8</text>
<text x="5.08" y="2.413" size="0.9906" layer="21" ratio="12">9</text>
<text x="7.62" y="2.413" size="0.9906" layer="21" ratio="12">10</text>
<text x="10.287" y="2.413" size="0.9906" layer="21" ratio="12">11</text>
<text x="12.954" y="2.413" size="0.9906" layer="21" ratio="12">12</text>
<text x="15.875" y="2.413" size="0.9906" layer="21" ratio="12">13</text>
<text x="-16.002" y="-3.429" size="0.9906" layer="21" ratio="12">14</text>
<text x="-13.208" y="-3.429" size="0.9906" layer="21" ratio="12">15</text>
<text x="-10.414" y="-3.429" size="0.9906" layer="21" ratio="12">16</text>
<text x="-7.747" y="-3.429" size="0.9906" layer="21" ratio="12">17</text>
<text x="-5.08" y="-3.429" size="0.9906" layer="21" ratio="12">18</text>
<text x="-2.159" y="-3.429" size="0.9906" layer="21" ratio="12">19</text>
<text x="0.508" y="-3.429" size="0.9906" layer="21" ratio="12">20</text>
<text x="3.302" y="-3.429" size="0.9906" layer="21" ratio="12">21</text>
<text x="6.096" y="-3.429" size="0.9906" layer="21" ratio="12">22</text>
<text x="8.89" y="-3.429" size="0.9906" layer="21" ratio="12">23</text>
<text x="11.557" y="-3.429" size="0.9906" layer="21" ratio="12">24</text>
<text x="14.351" y="-3.429" size="0.9906" layer="21" ratio="12">25</text>
<hole x="23.5204" y="0" drill="3.302"/>
<hole x="-23.5204" y="0" drill="3.302"/>
</package>
<package name="M25VP">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="-18.5674" y1="-2.9464" x2="-19.3548" y2="2.3368" width="0.1524" layer="21"/>
<wire x1="17.272" y1="-3.937" x2="18.5807" y2="-2.905" width="0.1524" layer="21" curve="76.489196"/>
<wire x1="18.034" y1="3.937" x2="19.3495" y2="2.3038" width="0.1524" layer="21" curve="-102.298925"/>
<wire x1="19.3548" y1="2.3114" x2="18.5674" y2="-2.921" width="0.1524" layer="21"/>
<wire x1="-18.034" y1="3.937" x2="18.034" y2="3.937" width="0.1524" layer="21"/>
<wire x1="-19.3541" y1="2.3268" x2="-18.034" y2="3.937" width="0.1524" layer="21" curve="-101.30773"/>
<wire x1="-18.5749" y1="-2.9295" x2="-17.272" y2="-3.937" width="0.1524" layer="21" curve="75.428151"/>
<wire x1="-17.272" y1="-3.937" x2="17.272" y2="-3.937" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.223" x2="-23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.223" x2="-21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.096" x2="-21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.096" x2="-21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.223" x2="21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-25.908" y1="-6.223" x2="-23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-5.588" x2="-25.908" y2="-6.223" width="0.1524" layer="21" curve="90"/>
<wire x1="25.908" y1="-6.223" x2="26.543" y2="-5.588" width="0.1524" layer="21" curve="90"/>
<wire x1="25.908" y1="6.223" x2="23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="26.543" y1="5.588" x2="26.543" y2="-5.588" width="0.1524" layer="21"/>
<wire x1="25.908" y1="6.223" x2="26.543" y2="5.588" width="0.1524" layer="21" curve="-90"/>
<wire x1="-26.543" y1="5.588" x2="-26.543" y2="-5.588" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="5.588" x2="-25.908" y2="6.223" width="0.1524" layer="21" curve="-90"/>
<wire x1="21.209" y1="-6.223" x2="21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.223" x2="23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.096" x2="23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.096" x2="23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.223" x2="25.908" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.223" x2="21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.223" x2="-21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.096" x2="23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.096" x2="23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.223" x2="21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.223" x2="-23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.223" x2="-25.908" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.096" x2="-21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.096" x2="-21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.223" x2="-23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-18.034" y1="4.318" x2="18.034" y2="4.318" width="0.1524" layer="21"/>
<wire x1="19.7358" y1="2.3114" x2="18.9738" y2="-2.8702" width="0.1524" layer="21"/>
<wire x1="18.034" y1="4.318" x2="19.728" y2="2.2521" width="0.1524" layer="21" curve="-101.297755"/>
<wire x1="-18.9484" y1="-2.9464" x2="-19.7358" y2="2.3368" width="0.1524" layer="21"/>
<wire x1="-19.7277" y1="2.2521" x2="-18.034" y2="4.318" width="0.1524" layer="21" curve="-101.307706"/>
<wire x1="-17.272" y1="-4.318" x2="17.272" y2="-4.318" width="0.1524" layer="21"/>
<wire x1="-18.984" y1="-2.8191" x2="-17.272" y2="-4.318" width="0.1524" layer="21" curve="82.402958"/>
<wire x1="17.272" y1="-4.318" x2="18.9963" y2="-2.6849" width="0.1524" layer="21" curve="86.865803"/>
<circle x="-23.5204" y="-0.0254" radius="1.651" width="0.1524" layer="21"/>
<circle x="23.5204" y="0" radius="1.651" width="0.1524" layer="21"/>
<circle x="-2.7686" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-4.1402" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-5.5118" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="0" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-1.3716" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="1.3716" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="2.7686" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="5.5118" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="4.1402" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="6.9088" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="8.2804" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="9.652" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="11.049" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="12.4206" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="13.7922" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="16.5608" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="15.1892" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-8.2804" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-6.9088" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-9.652" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-11.049" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-13.7922" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-16.5608" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-12.4206" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-15.1892" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<pad name="13" x="16.5608" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="12" x="13.7922" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="11" x="11.049" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="10" x="8.2804" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="9" x="5.5118" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="8" x="2.7686" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="7" x="0" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="6" x="-2.7686" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="5" x="-5.5118" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="4" x="-8.2804" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="3" x="-11.049" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="2" x="-13.7922" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="1" x="-16.5608" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="25" x="15.1892" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="24" x="12.4206" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="23" x="9.652" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="22" x="6.9088" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="21" x="4.1402" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="20" x="1.3716" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="19" x="-1.3716" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="18" x="-4.1402" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="17" x="-6.9088" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="16" x="-9.652" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="15" x="-12.4206" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="14" x="-15.1892" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="G1" x="-23.5204" y="0" drill="3.302" diameter="5.08"/>
<pad name="G2" x="23.5204" y="0" drill="3.302" diameter="5.08"/>
<text x="-26.289" y="6.985" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.54" y="6.985" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-16.764" y="2.413" size="0.9906" layer="21" ratio="12">1</text>
<text x="-14.097" y="2.413" size="0.9906" layer="21" ratio="12">2</text>
<text x="-11.43" y="2.413" size="0.9906" layer="21" ratio="12">3</text>
<text x="-8.763" y="2.413" size="0.9906" layer="21" ratio="12">4</text>
<text x="-5.969" y="2.413" size="0.9906" layer="21" ratio="12">5</text>
<text x="-3.175" y="2.413" size="0.9906" layer="21" ratio="12">6</text>
<text x="-0.381" y="2.413" size="0.9906" layer="21" ratio="12">7</text>
<text x="2.413" y="2.413" size="0.9906" layer="21" ratio="12">8</text>
<text x="5.08" y="2.413" size="0.9906" layer="21" ratio="12">9</text>
<text x="7.62" y="2.413" size="0.9906" layer="21" ratio="12">10</text>
<text x="10.287" y="2.413" size="0.9906" layer="21" ratio="12">11</text>
<text x="12.954" y="2.413" size="0.9906" layer="21" ratio="12">12</text>
<text x="15.875" y="2.413" size="0.9906" layer="21" ratio="12">13</text>
<text x="-16.002" y="-3.429" size="0.9906" layer="21" ratio="12">14</text>
<text x="-13.208" y="-3.429" size="0.9906" layer="21" ratio="12">15</text>
<text x="-10.414" y="-3.429" size="0.9906" layer="21" ratio="12">16</text>
<text x="-7.747" y="-3.429" size="0.9906" layer="21" ratio="12">17</text>
<text x="-5.08" y="-3.429" size="0.9906" layer="21" ratio="12">18</text>
<text x="-2.159" y="-3.429" size="0.9906" layer="21" ratio="12">19</text>
<text x="0.508" y="-3.429" size="0.9906" layer="21" ratio="12">20</text>
<text x="3.302" y="-3.429" size="0.9906" layer="21" ratio="12">21</text>
<text x="6.096" y="-3.429" size="0.9906" layer="21" ratio="12">22</text>
<text x="8.89" y="-3.429" size="0.9906" layer="21" ratio="12">23</text>
<text x="11.557" y="-3.429" size="0.9906" layer="21" ratio="12">24</text>
<text x="14.351" y="-3.429" size="0.9906" layer="21" ratio="12">25</text>
</package>
<package name="M25VB">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<wire x1="18.5674" y1="2.9464" x2="19.3548" y2="-2.3368" width="0.1524" layer="21"/>
<wire x1="-18.5807" y1="2.905" x2="-17.272" y2="3.937" width="0.1524" layer="21" curve="-76.489196"/>
<wire x1="-19.3495" y1="-2.3038" x2="-18.034" y2="-3.937" width="0.1524" layer="21" curve="102.298925"/>
<wire x1="-19.3548" y1="-2.3114" x2="-18.5674" y2="2.921" width="0.1524" layer="21"/>
<wire x1="18.034" y1="-3.937" x2="-18.034" y2="-3.937" width="0.1524" layer="21"/>
<wire x1="18.034" y1="-3.937" x2="19.3541" y2="-2.3268" width="0.1524" layer="21" curve="101.30773"/>
<wire x1="17.272" y1="3.937" x2="18.5749" y2="2.9295" width="0.1524" layer="21" curve="-75.428151"/>
<wire x1="17.272" y1="3.937" x2="-17.272" y2="3.937" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.223" x2="23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.223" x2="21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="6.096" x2="21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.096" x2="21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="6.223" x2="-21.209" y2="6.223" width="0.1524" layer="21"/>
<wire x1="25.908" y1="6.223" x2="23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="25.908" y1="6.223" x2="26.543" y2="5.588" width="0.1524" layer="21" curve="-90"/>
<wire x1="-26.543" y1="5.588" x2="-25.908" y2="6.223" width="0.1524" layer="21" curve="-90"/>
<wire x1="-25.908" y1="-6.223" x2="-23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-5.588" x2="-26.543" y2="5.588" width="0.1524" layer="21"/>
<wire x1="-26.543" y1="-5.588" x2="-25.908" y2="-6.223" width="0.1524" layer="21" curve="90"/>
<wire x1="26.543" y1="-5.588" x2="26.543" y2="5.588" width="0.1524" layer="21"/>
<wire x1="25.908" y1="-6.223" x2="26.543" y2="-5.588" width="0.1524" layer="21" curve="90"/>
<wire x1="-21.209" y1="6.223" x2="-21.209" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.223" x2="-23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="6.096" x2="-23.749" y2="6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.096" x2="-23.749" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="6.223" x2="-25.908" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.223" x2="-21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.223" x2="21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-21.209" y1="-6.096" x2="-23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.096" x2="-23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-23.749" y1="-6.223" x2="-21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.223" x2="23.749" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.223" x2="25.908" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="23.749" y1="-6.096" x2="21.209" y2="-6.096" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.096" x2="21.209" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="21.209" y1="-6.223" x2="23.749" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="18.034" y1="-4.318" x2="-18.034" y2="-4.318" width="0.1524" layer="21"/>
<wire x1="-19.7358" y1="-2.3114" x2="-18.9738" y2="2.8702" width="0.1524" layer="21"/>
<wire x1="-19.728" y1="-2.2521" x2="-18.034" y2="-4.318" width="0.1524" layer="21" curve="101.297755"/>
<wire x1="18.9484" y1="2.9464" x2="19.7358" y2="-2.3368" width="0.1524" layer="21"/>
<wire x1="18.034" y1="-4.318" x2="19.7277" y2="-2.2521" width="0.1524" layer="21" curve="101.307706"/>
<wire x1="17.272" y1="4.318" x2="-17.272" y2="4.318" width="0.1524" layer="21"/>
<wire x1="17.272" y1="4.318" x2="18.984" y2="2.8191" width="0.1524" layer="21" curve="-82.402958"/>
<wire x1="-18.9963" y1="2.6849" x2="-17.272" y2="4.318" width="0.1524" layer="21" curve="-86.865803"/>
<circle x="2.7686" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="4.1402" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="5.5118" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="0" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="1.3716" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-1.3716" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-2.7686" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-5.5118" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-4.1402" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-6.9088" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-8.2804" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-9.652" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-11.049" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-12.4206" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-13.7922" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-16.5608" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="-15.1892" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="8.2804" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="6.9088" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="9.652" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="11.049" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="13.7922" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="16.5608" y="-1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="12.4206" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<circle x="15.1892" y="1.4224" radius="0.254" width="0.4064" layer="51"/>
<pad name="13" x="-16.5608" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="12" x="-13.7922" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="11" x="-11.049" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="10" x="-8.2804" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="9" x="-5.5118" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="8" x="-2.7686" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="7" x="0" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="6" x="2.7686" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="5" x="5.5118" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="4" x="8.2804" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="3" x="11.049" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="2" x="13.7922" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="1" x="16.5608" y="-1.4224" drill="1.016" shape="octagon"/>
<pad name="25" x="-15.1892" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="24" x="-12.4206" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="23" x="-9.652" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="22" x="-6.9088" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="21" x="-4.1402" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="20" x="-1.3716" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="19" x="1.3716" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="18" x="4.1402" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="17" x="6.9088" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="16" x="9.652" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="15" x="12.4206" y="1.4224" drill="1.016" shape="octagon"/>
<pad name="14" x="15.1892" y="1.4224" drill="1.016" shape="octagon"/>
<text x="-26.289" y="6.985" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.54" y="6.985" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="16.764" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">1</text>
<text x="14.097" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">2</text>
<text x="11.43" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">3</text>
<text x="8.763" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">4</text>
<text x="5.969" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">5</text>
<text x="3.175" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">6</text>
<text x="0.381" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">7</text>
<text x="-2.413" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">8</text>
<text x="-5.08" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">9</text>
<text x="-7.62" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">10</text>
<text x="-10.287" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">11</text>
<text x="-12.954" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">12</text>
<text x="-15.875" y="-2.413" size="0.9906" layer="21" ratio="12" rot="R180">13</text>
<text x="16.002" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">14</text>
<text x="13.208" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">15</text>
<text x="10.414" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">16</text>
<text x="7.747" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">17</text>
<text x="5.08" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">18</text>
<text x="2.159" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">19</text>
<text x="-0.508" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">20</text>
<text x="-3.302" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">21</text>
<text x="-6.096" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">22</text>
<text x="-8.89" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">23</text>
<text x="-11.557" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">24</text>
<text x="-14.351" y="3.429" size="0.9906" layer="21" ratio="12" rot="R180">25</text>
</package>
<package name="182-025-MALE">
<description>&lt;b&gt;NORCOMP Right Angle D-Sub Connector&lt;/b&gt;, 182 Economy Series (.318" Footprint)&lt;p&gt;
Source: http://www.norcomp.net/ .. 182-yyy-113Ryy1Rev3.pdf</description>
<wire x1="19.3294" y1="-5.751" x2="19.8374" y2="-5.243" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.8374" y1="-5.243" x2="-19.3294" y2="-5.751" width="0.1524" layer="21" curve="90"/>
<wire x1="26.5684" y1="12.875" x2="26.5684" y2="2.885" width="0.1524" layer="21"/>
<wire x1="26.5684" y1="2.885" x2="26.5684" y2="0.091" width="0.1524" layer="21"/>
<wire x1="-26.5684" y1="12.875" x2="-26.5684" y2="0.091" width="0.1524" layer="21"/>
<wire x1="26.5684" y1="0.091" x2="-26.5684" y2="0.091" width="0.1524" layer="21"/>
<wire x1="-19.8374" y1="-0.544" x2="-19.8374" y2="-5.243" width="0.1524" layer="21"/>
<wire x1="-20.4724" y1="0.091" x2="-19.8374" y2="-0.544" width="0.1524" layer="21" curve="-90"/>
<wire x1="19.8374" y1="-0.544" x2="19.8374" y2="-5.243" width="0.1524" layer="21"/>
<wire x1="19.8374" y1="-0.544" x2="20.4724" y2="0.091" width="0.1524" layer="21" curve="-90"/>
<wire x1="19.3294" y1="-5.751" x2="-19.3294" y2="-5.751" width="0.1524" layer="21"/>
<wire x1="26.5684" y1="2.885" x2="-26.5684" y2="2.885" width="0.1524" layer="21"/>
<wire x1="-20.5994" y1="12.875" x2="-20.5994" y2="3.082" width="0.1524" layer="21"/>
<wire x1="20.5994" y1="3.082" x2="20.5994" y2="12.875" width="0.1524" layer="21"/>
<wire x1="26.5684" y1="12.8926" x2="-26.5684" y2="12.8926" width="0.1524" layer="21"/>
<pad name="5" x="-5.5372" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="4" x="-8.3058" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="3" x="-11.0744" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="2" x="-13.843" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="1" x="-16.6116" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="14" x="-15.2273" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="6" x="-2.7686" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="7" x="0" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="8" x="2.7686" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="9" x="5.5372" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="10" x="8.3058" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="11" x="11.0744" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="12" x="13.843" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="13" x="16.6116" y="10.9233" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="15" x="-12.4587" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="16" x="-9.6901" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="17" x="-6.9215" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="18" x="-4.1529" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="19" x="-1.3843" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="20" x="1.3843" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="21" x="4.1529" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="22" x="6.9215" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="23" x="9.6901" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="24" x="12.4587" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<pad name="25" x="15.2273" y="8.0797" drill="1.1938" diameter="1.524" shape="octagon"/>
<text x="-20.32" y="13.335" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-8.89" y="13.335" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-18.6944" y="9.9782" size="1.27" layer="21" ratio="10">1</text>
<text x="18.3134" y="9.9782" size="1.27" layer="21" ratio="10">13</text>
<text x="18.3134" y="7.4382" size="1.27" layer="21" ratio="10">25</text>
<text x="-18.6944" y="7.3112" size="1.27" layer="21" ratio="10">14</text>
<hole x="-23.5204" y="9.5009" drill="3.048"/>
<hole x="23.5204" y="9.5009" drill="3.048"/>
</package>
<package name="M25SMT">
<description>&lt;b&gt;D-Sub Steckverbinder&lt;/b&gt; Abgewinkelte Messerleisten SMT, Einbauhöhe 3,6mm&lt;p&gt;
Source: ERNI-D-Sub-Conmnectors-d.pdf / www.erni.com</description>
<wire x1="19.279" y1="-17.526" x2="19.025" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.279" y1="-17.526" x2="19.787" y2="-17.018" width="0.1524" layer="21" curve="90"/>
<wire x1="-19.787" y1="-17.018" x2="-19.279" y2="-17.526" width="0.1524" layer="21" curve="90"/>
<wire x1="19.406" y1="-11.684" x2="19.406" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="19.406" y1="-11.684" x2="-19.406" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-19.406" y1="-11.684" x2="-19.406" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="26.518" y1="-8.92" x2="20.295" y2="-8.92" width="0.1524" layer="21"/>
<wire x1="26.518" y1="-8.92" x2="26.518" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="26.518" y1="-8.92" x2="26.47" y2="-8.793" width="0.1524" layer="21"/>
<wire x1="-26.518" y1="-11.684" x2="-26.518" y2="-8.92" width="0.1524" layer="21"/>
<wire x1="-26.518" y1="-8.92" x2="-26.47" y2="-8.793" width="0.1524" layer="21"/>
<wire x1="20.295" y1="-8.92" x2="20.295" y2="-8.158" width="0.1524" layer="21"/>
<wire x1="20.295" y1="-8.92" x2="-20.295" y2="-8.92" width="0.1524" layer="21"/>
<wire x1="20.295" y1="-8.158" x2="-20.295" y2="-8.158" width="0.1524" layer="21"/>
<wire x1="-20.295" y1="-8.92" x2="-20.295" y2="-8.158" width="0.1524" layer="21"/>
<wire x1="26.47" y1="3.016" x2="26.47" y2="-8.793" width="0.1524" layer="21"/>
<wire x1="-26.47" y1="3.016" x2="-26.47" y2="-8.793" width="0.1524" layer="21"/>
<wire x1="2.769" y1="-13.97" x2="5.055" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="5.055" y1="-13.97" x2="5.055" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="5.055" y1="-16.256" x2="4.674" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="4.674" y1="-16.256" x2="4.674" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="4.674" y1="-14.351" x2="3.15" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="3.15" y1="-14.351" x2="3.15" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="3.15" y1="-16.256" x2="2.769" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="2.769" y1="-16.256" x2="2.769" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-20.295" y1="-8.92" x2="-26.518" y2="-8.92" width="0.1524" layer="21"/>
<wire x1="-19.406" y1="-11.684" x2="-26.518" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="-19.787" y1="-12.319" x2="-19.787" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="-20.422" y1="-11.684" x2="-19.787" y2="-12.319" width="0.1524" layer="21" curve="-90"/>
<wire x1="26.518" y1="-11.684" x2="19.406" y2="-11.684" width="0.1524" layer="21"/>
<wire x1="19.787" y1="-12.319" x2="19.787" y2="-17.018" width="0.1524" layer="21"/>
<wire x1="19.787" y1="-12.319" x2="20.422" y2="-11.684" width="0.1524" layer="21" curve="-90"/>
<wire x1="6.706" y1="-13.97" x2="8.992" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="8.992" y1="-13.97" x2="8.992" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="8.992" y1="-16.256" x2="8.611" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="8.611" y1="-16.256" x2="8.611" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="8.611" y1="-14.351" x2="7.087" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="7.087" y1="-14.351" x2="7.087" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="7.087" y1="-16.256" x2="6.706" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="6.706" y1="-16.256" x2="6.706" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="10.643" y1="-13.97" x2="12.929" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="12.929" y1="-13.97" x2="12.929" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="12.929" y1="-16.256" x2="12.548" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="12.548" y1="-16.256" x2="12.548" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="12.548" y1="-14.351" x2="11.024" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="11.024" y1="-14.351" x2="11.024" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="11.024" y1="-16.256" x2="10.643" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="10.643" y1="-16.256" x2="10.643" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-13.36" y1="-13.97" x2="-11.074" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-11.074" y1="-13.97" x2="-11.074" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-11.074" y1="-16.256" x2="-11.455" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-11.455" y1="-16.256" x2="-11.455" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-11.455" y1="-14.351" x2="-12.979" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-12.979" y1="-14.351" x2="-12.979" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-12.979" y1="-16.256" x2="-13.36" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-13.36" y1="-16.256" x2="-13.36" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-9.423" y1="-13.97" x2="-7.137" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-7.137" y1="-13.97" x2="-7.137" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-7.137" y1="-16.256" x2="-7.518" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-7.518" y1="-16.256" x2="-7.518" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-7.518" y1="-14.351" x2="-9.042" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-9.042" y1="-14.351" x2="-9.042" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-9.042" y1="-16.256" x2="-9.423" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-9.423" y1="-16.256" x2="-9.423" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-5.486" y1="-13.97" x2="-3.2" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-3.2" y1="-13.97" x2="-3.2" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-3.2" y1="-16.256" x2="-3.581" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-3.581" y1="-16.256" x2="-3.581" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-3.581" y1="-14.351" x2="-5.105" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-5.105" y1="-14.351" x2="-5.105" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-5.105" y1="-16.256" x2="-5.486" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-5.486" y1="-16.256" x2="-5.486" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-1.295" y1="-13.97" x2="0.991" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="0.991" y1="-13.97" x2="0.991" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="0.991" y1="-16.256" x2="0.61" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="0.61" y1="-16.256" x2="0.61" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="0.61" y1="-14.351" x2="-0.914" y2="-14.351" width="0.1524" layer="21"/>
<wire x1="-0.914" y1="-14.351" x2="-0.914" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-0.914" y1="-16.256" x2="-1.295" y2="-16.256" width="0.1524" layer="21"/>
<wire x1="-1.295" y1="-16.256" x2="-1.295" y2="-13.97" width="0.1524" layer="21"/>
<wire x1="-19.025" y1="-12.192" x2="-19.025" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="-19.025" y1="-17.526" x2="-19.279" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.025" y1="-12.192" x2="19.025" y2="-17.526" width="0.1524" layer="21"/>
<wire x1="19.025" y1="-17.526" x2="-19.025" y2="-17.526" width="0.1524" layer="21"/>
<text x="13.6525" y="5.3975" size="1.778" layer="25" ratio="10">&gt;NAME</text>
<text x="-22.5425" y="5.3975" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
<text x="-19.685" y="3.4925" size="1.27" layer="21" ratio="10">13</text>
<text x="18.0975" y="3.4925" size="1.27" layer="21" ratio="10">1</text>
<text x="-18.812" y="-2.682" size="1.27" layer="21" ratio="10">25</text>
<text x="17.153" y="-2.38" size="1.27" layer="21" ratio="10">14</text>
<text x="-18.319" y="-15.104" size="1.27" layer="21" ratio="10">M25</text>
<rectangle x1="-20.3" y1="-8.92" x2="20.275" y2="-8.158" layer="21"/>
<rectangle x1="-26.55" y1="-11.684" x2="26.525" y2="-11.04" layer="21"/>
<hole x="-22.57" y="0" drill="1.7"/>
<hole x="22.57" y="0" drill="1.7"/>
<smd name="1" x="16.56" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="14" x="15.18" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="2" x="13.8" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="3" x="11.04" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="4" x="8.28" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="5" x="5.52" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="6" x="2.76" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="7" x="0" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="8" x="-2.76" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="9" x="-5.52" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="10" x="-8.28" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="11" x="-11.04" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="12" x="-13.8" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="13" x="-16.56" y="1.975" dx="1" dy="2.8" layer="1"/>
<smd name="S@2" x="-23.445" y="-2.625" dx="1" dy="1" layer="1"/>
<smd name="S@1" x="23.445" y="-2.65" dx="1" dy="1" layer="1"/>
<wire x1="-26.6475" y1="-8.1" x2="26.6475" y2="-8.1" width="0" layer="20"/>
<polygon width="0.2" layer="1">
<vertex x="-26.3275" y="-7.9375"/>
<vertex x="-20.52" y="-7.9375"/>
<vertex x="-20.52" y="-1.5875"/>
<vertex x="-22.5175" y="-1.5875" curve="-180"/>
<vertex x="-22.5175" y="1.5875"/>
<vertex x="-20.495" y="1.5875"/>
<vertex x="-20.495" y="3.325"/>
<vertex x="-26.3275" y="3.325"/>
</polygon>
<polygon width="0.2" layer="1">
<vertex x="26.3275" y="-7.9375"/>
<vertex x="20.52" y="-7.9375"/>
<vertex x="20.52" y="-1.5875"/>
<vertex x="22.5175" y="-1.5875" curve="180"/>
<vertex x="22.5175" y="1.5875"/>
<vertex x="20.495" y="1.5875"/>
<vertex x="20.495" y="3.325"/>
<vertex x="26.3275" y="3.325"/>
</polygon>
<wire x1="26.47" y1="3.016" x2="-26.47" y2="3.016" width="0.1524" layer="21"/>
<smd name="15" x="12.42" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="16" x="9.66" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="17" x="6.9" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="18" x="4.14" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="19" x="1.38" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="20" x="-1.38" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="21" x="-4.14" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="22" x="-6.9" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="23" x="-9.66" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="24" x="-12.42" y="-1.975" dx="1" dy="2.8" layer="1"/>
<smd name="25" x="-15.18" y="-1.975" dx="1" dy="2.8" layer="1"/>
</package>
</packages>
<symbols>
<symbol name="M25">
<wire x1="1.27" y1="12.7" x2="2.54" y2="12.7" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="12.7" x2="-1.27" y2="12.7" width="0.6096" layer="94"/>
<wire x1="1.27" y1="7.62" x2="2.54" y2="7.62" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="10.16" x2="-1.27" y2="10.16" width="0.6096" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="7.62" x2="-1.27" y2="7.62" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="-1.27" y2="5.08" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-7.62" x2="2.54" y2="-7.62" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-1.27" y2="2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-12.7" x2="2.54" y2="-12.7" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.27" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-17.78" x2="2.54" y2="-17.78" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-1.27" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="10.16" x2="2.54" y2="10.16" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-5.08" x2="-1.27" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="1.27" y1="5.08" x2="2.54" y2="5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-7.62" x2="-1.27" y2="-7.62" width="0.6096" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-10.16" x2="-1.27" y2="-10.16" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="2.54" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-12.7" x2="-1.27" y2="-12.7" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-10.16" x2="2.54" y2="-10.16" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-15.24" x2="-1.27" y2="-15.24" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-15.24" x2="2.54" y2="-15.24" width="0.6096" layer="94"/>
<wire x1="2.5226" y1="-20.8718" x2="4.0637" y2="-19.6312" width="0.4064" layer="94" curve="102.322193" cap="flat"/>
<wire x1="2.5226" y1="-20.8718" x2="-3.0654" y2="-19.6494" width="0.4064" layer="94"/>
<wire x1="-4.064" y1="-18.4088" x2="-3.0654" y2="-19.6494" width="0.4064" layer="94" curve="77.657889"/>
<wire x1="-4.064" y1="13.3288" x2="-4.064" y2="-18.4088" width="0.4064" layer="94"/>
<wire x1="-4.064" y1="13.3288" x2="-3.0654" y2="14.5694" width="0.4064" layer="94" curve="-77.657889"/>
<wire x1="4.064" y1="14.5512" x2="4.064" y2="-19.6312" width="0.4064" layer="94"/>
<wire x1="2.5226" y1="15.7918" x2="-3.0654" y2="14.5694" width="0.4064" layer="94"/>
<wire x1="2.5226" y1="15.7919" x2="4.064" y2="14.5512" width="0.4064" layer="94" curve="-102.337599" cap="flat"/>
<text x="-3.81" y="-23.495" size="1.778" layer="96">&gt;VALUE</text>
<text x="-3.81" y="16.51" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="12.7" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="14" x="-7.62" y="12.7" visible="pad" length="middle" direction="pas"/>
<pin name="2" x="7.62" y="10.16" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="15" x="-7.62" y="10.16" visible="pad" length="middle" direction="pas"/>
<pin name="3" x="7.62" y="7.62" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="16" x="-7.62" y="7.62" visible="pad" length="middle" direction="pas"/>
<pin name="4" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="17" x="-7.62" y="5.08" visible="pad" length="middle" direction="pas"/>
<pin name="5" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="18" x="-7.62" y="2.54" visible="pad" length="middle" direction="pas"/>
<pin name="6" x="7.62" y="0" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="19" x="-7.62" y="0" visible="pad" length="middle" direction="pas"/>
<pin name="7" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="20" x="-7.62" y="-2.54" visible="pad" length="middle" direction="pas"/>
<pin name="8" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="21" x="-7.62" y="-5.08" visible="pad" length="middle" direction="pas"/>
<pin name="9" x="7.62" y="-7.62" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="22" x="-7.62" y="-7.62" visible="pad" length="middle" direction="pas"/>
<pin name="10" x="7.62" y="-10.16" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="23" x="-7.62" y="-10.16" visible="pad" length="middle" direction="pas"/>
<pin name="11" x="7.62" y="-12.7" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="24" x="-7.62" y="-12.7" visible="pad" length="middle" direction="pas"/>
<pin name="12" x="7.62" y="-15.24" visible="pad" length="middle" direction="pas" rot="R180"/>
<pin name="25" x="-7.62" y="-15.24" visible="pad" length="middle" direction="pas"/>
<pin name="13" x="7.62" y="-17.78" visible="pad" length="middle" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="M25" prefix="X" uservalue="yes">
<description>&lt;b&gt;SUB-D&lt;/b&gt;</description>
<gates>
<gate name="-1" symbol="M25" x="0" y="0"/>
</gates>
<devices>
<device name="D" package="M25D">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="H" package="M25H">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="HP" package="M25HP">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="V" package="M25V">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="VP" package="M25VP">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="VB" package="M25VB">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="-182" package="182-025-MALE">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="SMT" package="M25SMT">
<connects>
<connect gate="-1" pin="1" pad="1"/>
<connect gate="-1" pin="10" pad="10"/>
<connect gate="-1" pin="11" pad="11"/>
<connect gate="-1" pin="12" pad="12"/>
<connect gate="-1" pin="13" pad="13"/>
<connect gate="-1" pin="14" pad="14"/>
<connect gate="-1" pin="15" pad="15"/>
<connect gate="-1" pin="16" pad="16"/>
<connect gate="-1" pin="17" pad="17"/>
<connect gate="-1" pin="18" pad="18"/>
<connect gate="-1" pin="19" pad="19"/>
<connect gate="-1" pin="2" pad="2"/>
<connect gate="-1" pin="20" pad="20"/>
<connect gate="-1" pin="21" pad="21"/>
<connect gate="-1" pin="22" pad="22"/>
<connect gate="-1" pin="23" pad="23"/>
<connect gate="-1" pin="24" pad="24"/>
<connect gate="-1" pin="25" pad="25"/>
<connect gate="-1" pin="3" pad="3"/>
<connect gate="-1" pin="4" pad="4"/>
<connect gate="-1" pin="5" pad="5"/>
<connect gate="-1" pin="6" pad="6"/>
<connect gate="-1" pin="7" pad="7"/>
<connect gate="-1" pin="8" pad="8"/>
<connect gate="-1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="ic-package">
<description>&lt;b&gt;IC Packages an Sockets&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DIL32">
<description>&lt;b&gt;Dual In Line Package&lt;/b&gt;</description>
<wire x1="20.32" y1="6.731" x2="-20.32" y2="6.731" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-6.731" x2="20.32" y2="-6.731" width="0.1524" layer="21"/>
<wire x1="20.32" y1="6.731" x2="20.32" y2="-6.731" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="6.731" x2="-20.32" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="-6.731" x2="-20.32" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-20.32" y1="1.016" x2="-20.32" y2="-1.016" width="0.1524" layer="21" curve="-180"/>
<pad name="1" x="-19.05" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="-3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="-1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="19.05" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="19.05" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="21" x="8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="22" x="6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="23" x="3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="24" x="1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="25" x="-1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="26" x="-3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="27" x="-6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="28" x="-8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="29" x="-11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="30" x="-13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="31" x="-16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="32" x="-19.05" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<text x="-20.701" y="-6.604" size="1.778" layer="25" rot="R90">&gt;NAME</text>
<text x="-16.891" y="-0.889" size="1.778" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="DIL32">
<wire x1="-5.08" y1="19.05" x2="-5.08" y2="-21.59" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-21.59" x2="5.08" y2="-21.59" width="0.254" layer="94"/>
<wire x1="5.08" y1="-21.59" x2="5.08" y2="19.05" width="0.254" layer="94"/>
<wire x1="5.08" y1="19.05" x2="2.54" y2="19.05" width="0.254" layer="94"/>
<wire x1="-5.08" y1="19.05" x2="-2.54" y2="19.05" width="0.254" layer="94"/>
<wire x1="-2.54" y1="19.05" x2="2.54" y2="19.05" width="0.254" layer="94" curve="180"/>
<text x="-4.445" y="19.685" size="1.778" layer="95">&gt;NAME</text>
<text x="-4.445" y="-24.13" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-7.62" y="17.78" visible="pad" length="short" direction="pas"/>
<pin name="2" x="-7.62" y="15.24" visible="pad" length="short" direction="pas"/>
<pin name="3" x="-7.62" y="12.7" visible="pad" length="short" direction="pas"/>
<pin name="4" x="-7.62" y="10.16" visible="pad" length="short" direction="pas"/>
<pin name="5" x="-7.62" y="7.62" visible="pad" length="short" direction="pas"/>
<pin name="6" x="-7.62" y="5.08" visible="pad" length="short" direction="pas"/>
<pin name="7" x="-7.62" y="2.54" visible="pad" length="short" direction="pas"/>
<pin name="8" x="-7.62" y="0" visible="pad" length="short" direction="pas"/>
<pin name="9" x="-7.62" y="-2.54" visible="pad" length="short" direction="pas"/>
<pin name="10" x="-7.62" y="-5.08" visible="pad" length="short" direction="pas"/>
<pin name="11" x="-7.62" y="-7.62" visible="pad" length="short" direction="pas"/>
<pin name="12" x="-7.62" y="-10.16" visible="pad" length="short" direction="pas"/>
<pin name="13" x="-7.62" y="-12.7" visible="pad" length="short" direction="pas"/>
<pin name="14" x="-7.62" y="-15.24" visible="pad" length="short" direction="pas"/>
<pin name="15" x="-7.62" y="-17.78" visible="pad" length="short" direction="pas"/>
<pin name="16" x="-7.62" y="-20.32" visible="pad" length="short" direction="pas"/>
<pin name="17" x="7.62" y="-20.32" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="18" x="7.62" y="-17.78" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="19" x="7.62" y="-15.24" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="20" x="7.62" y="-12.7" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="21" x="7.62" y="-10.16" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="22" x="7.62" y="-7.62" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="23" x="7.62" y="-5.08" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="24" x="7.62" y="-2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="25" x="7.62" y="0" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="26" x="7.62" y="2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="27" x="7.62" y="5.08" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="28" x="7.62" y="7.62" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="29" x="7.62" y="10.16" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="30" x="7.62" y="12.7" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="31" x="7.62" y="15.24" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="32" x="7.62" y="17.78" visible="pad" length="short" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="DIL32" prefix="IC" uservalue="yes">
<description>&lt;b&gt;Dual In Line&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="DIL32" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DIL32">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="11" pad="11"/>
<connect gate="G$1" pin="12" pad="12"/>
<connect gate="G$1" pin="13" pad="13"/>
<connect gate="G$1" pin="14" pad="14"/>
<connect gate="G$1" pin="15" pad="15"/>
<connect gate="G$1" pin="16" pad="16"/>
<connect gate="G$1" pin="17" pad="17"/>
<connect gate="G$1" pin="18" pad="18"/>
<connect gate="G$1" pin="19" pad="19"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="20" pad="20"/>
<connect gate="G$1" pin="21" pad="21"/>
<connect gate="G$1" pin="22" pad="22"/>
<connect gate="G$1" pin="23" pad="23"/>
<connect gate="G$1" pin="24" pad="24"/>
<connect gate="G$1" pin="25" pad="25"/>
<connect gate="G$1" pin="26" pad="26"/>
<connect gate="G$1" pin="27" pad="27"/>
<connect gate="G$1" pin="28" pad="28"/>
<connect gate="G$1" pin="29" pad="29"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="30" pad="30"/>
<connect gate="G$1" pin="31" pad="31"/>
<connect gate="G$1" pin="32" pad="32"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="microchip">
<description>&lt;b&gt;Microchip PIC Microcontrollers and other Devices&lt;/b&gt;&lt;p&gt;
Based on the following sources :
&lt;ul&gt;
&lt;li&gt;Microchip Data Book, 1993
&lt;li&gt;THE EMERGING WORLD STANDARD, 1995/1996
&lt;li&gt;Microchip, Technical Library CD-ROM, June 1998
&lt;li&gt;www.microchip.com
&lt;/ul&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DIL28-6">
<description>&lt;B&gt;Dual In Line&lt;/B&gt;&lt;p&gt;
package type P</description>
<wire x1="-18.542" y1="1.27" x2="-18.542" y2="-1.27" width="0.1524" layer="21" curve="-180"/>
<wire x1="18.669" y1="-6.223" x2="18.669" y2="6.223" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="6.223" x2="-18.542" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="6.223" x2="18.669" y2="6.223" width="0.1524" layer="21"/>
<wire x1="18.669" y1="-6.223" x2="-18.542" y2="-6.223" width="0.1524" layer="21"/>
<wire x1="-18.542" y1="-1.27" x2="-18.542" y2="-6.223" width="0.1524" layer="21"/>
<pad name="1" x="-16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="-1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="21" x="1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="22" x="-1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="23" x="-3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="24" x="-6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="25" x="-8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="26" x="-11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="27" x="-13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="28" x="-16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<text x="-18.923" y="-6.096" size="1.778" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="-13.97" y="-2.2098" size="1.778" layer="27" ratio="10">&gt;VALUE</text>
</package>
<package name="PLCC-32">
<description>&lt;b&gt;PLASTIC LEADED CHIP CARRIER&lt;/b&gt;&lt;p&gt;
rectangle, package type L</description>
<wire x1="-9.903" y1="9.903" x2="9.903" y2="9.903" width="0.1998" layer="39"/>
<wire x1="9.903" y1="9.903" x2="9.903" y2="-9.903" width="0.1998" layer="39"/>
<wire x1="9.903" y1="-9.903" x2="-9.903" y2="-9.903" width="0.1998" layer="39"/>
<wire x1="-9.903" y1="-9.903" x2="-9.903" y2="9.903" width="0.1998" layer="39"/>
<wire x1="-7.43" y1="5.195" x2="-7.43" y2="-4.306" width="0.2032" layer="51"/>
<wire x1="-5.576" y1="-6.16" x2="6.465" y2="-6.16" width="0.2032" layer="51"/>
<wire x1="7.43" y1="-5.195" x2="7.43" y2="5.195" width="0.2032" layer="51"/>
<wire x1="6.465" y1="6.16" x2="-6.465" y2="6.16" width="0.2032" layer="51"/>
<wire x1="-7.43" y1="-4.306" x2="-5.576" y2="-6.16" width="0.2032" layer="21"/>
<wire x1="-6.465" y1="6.16" x2="-7.43" y2="5.195" width="0.2032" layer="21"/>
<wire x1="6.465" y1="-6.16" x2="7.43" y2="-5.195" width="0.2032" layer="21"/>
<wire x1="7.43" y1="5.195" x2="6.465" y2="6.16" width="0.2032" layer="21"/>
<wire x1="-5.525" y1="6.084" x2="-5.525" y2="-6.084" width="0.2032" layer="21"/>
<circle x="-6.3299" y="0" radius="0.3" width="0.6096" layer="51"/>
<smd name="1" x="-6.83" y="0" dx="2.2" dy="0.6" layer="1"/>
<smd name="2" x="-6.83" y="-1.27" dx="2.2" dy="0.6" layer="1"/>
<smd name="3" x="-6.83" y="-2.54" dx="2.2" dy="0.6" layer="1"/>
<smd name="4" x="-6.83" y="-3.81" dx="2.2" dy="0.6" layer="1"/>
<smd name="5" x="-5.08" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="6" x="-3.81" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="7" x="-2.54" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="8" x="-1.27" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="9" x="0" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="10" x="1.27" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="11" x="2.54" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="12" x="3.81" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="13" x="5.08" y="-5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="14" x="6.83" y="-3.81" dx="2.2" dy="0.6" layer="1"/>
<smd name="15" x="6.83" y="-2.54" dx="2.2" dy="0.6" layer="1"/>
<smd name="16" x="6.83" y="-1.27" dx="2.2" dy="0.6" layer="1"/>
<smd name="17" x="6.83" y="0" dx="2.2" dy="0.6" layer="1"/>
<smd name="18" x="6.83" y="1.27" dx="2.2" dy="0.6" layer="1"/>
<smd name="19" x="6.83" y="2.54" dx="2.2" dy="0.6" layer="1"/>
<smd name="20" x="6.83" y="3.81" dx="2.2" dy="0.6" layer="1"/>
<smd name="21" x="5.08" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="22" x="3.81" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="23" x="2.54" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="24" x="1.27" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="25" x="0" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="26" x="-1.27" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="27" x="-2.54" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="28" x="-3.81" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="29" x="-5.08" y="5.56" dx="0.6" dy="2.2" layer="1"/>
<smd name="30" x="-6.83" y="3.81" dx="2.2" dy="0.6" layer="1"/>
<smd name="31" x="-6.83" y="2.54" dx="2.2" dy="0.6" layer="1"/>
<smd name="32" x="-6.83" y="1.27" dx="2.2" dy="0.6" layer="1"/>
<text x="-5.491" y="6.985" size="1.778" layer="25">&gt;NAME</text>
<text x="-5.461" y="-8.7551" size="1.778" layer="27">&gt;VALUE</text>
<rectangle x1="-7.9299" y1="-0.2601" x2="-7.48" y2="0.2601" layer="51"/>
<rectangle x1="-7.9299" y1="-1.5301" x2="-7.48" y2="-1.0099" layer="51"/>
<rectangle x1="-7.9299" y1="-2.8001" x2="-7.48" y2="-2.2799" layer="51"/>
<rectangle x1="-7.9299" y1="-4.0701" x2="-7.48" y2="-3.5499" layer="51"/>
<rectangle x1="-5.3401" y1="-6.6599" x2="-4.8199" y2="-6.21" layer="51"/>
<rectangle x1="-4.0701" y1="-6.6599" x2="-3.5499" y2="-6.21" layer="51"/>
<rectangle x1="-2.8001" y1="-6.6599" x2="-2.2799" y2="-6.21" layer="51"/>
<rectangle x1="-1.5301" y1="-6.6599" x2="-1.0099" y2="-6.21" layer="51"/>
<rectangle x1="-0.2601" y1="-6.6599" x2="0.2601" y2="-6.21" layer="51"/>
<rectangle x1="1.0099" y1="-6.6599" x2="1.5301" y2="-6.21" layer="51"/>
<rectangle x1="2.2799" y1="-6.6599" x2="2.8001" y2="-6.21" layer="51"/>
<rectangle x1="3.5499" y1="-6.6599" x2="4.0701" y2="-6.21" layer="51"/>
<rectangle x1="4.8199" y1="-6.6599" x2="5.3401" y2="-6.21" layer="51"/>
<rectangle x1="7.48" y1="-4.0701" x2="7.9299" y2="-3.5499" layer="51"/>
<rectangle x1="7.48" y1="-2.8001" x2="7.9299" y2="-2.2799" layer="51"/>
<rectangle x1="7.48" y1="-1.5301" x2="7.9299" y2="-1.0099" layer="51"/>
<rectangle x1="7.48" y1="-0.2601" x2="7.9299" y2="0.2601" layer="51"/>
<rectangle x1="7.48" y1="1.0099" x2="7.9299" y2="1.5301" layer="51"/>
<rectangle x1="7.48" y1="2.2799" x2="7.9299" y2="2.8001" layer="51"/>
<rectangle x1="7.48" y1="3.5499" x2="7.9299" y2="4.0701" layer="51"/>
<rectangle x1="4.8199" y1="6.21" x2="5.3401" y2="6.6599" layer="51"/>
<rectangle x1="3.5499" y1="6.21" x2="4.0701" y2="6.6599" layer="51"/>
<rectangle x1="2.2799" y1="6.21" x2="2.8001" y2="6.6599" layer="51"/>
<rectangle x1="1.0099" y1="6.21" x2="1.5301" y2="6.6599" layer="51"/>
<rectangle x1="-0.2601" y1="6.21" x2="0.2601" y2="6.6599" layer="51"/>
<rectangle x1="-1.5301" y1="6.21" x2="-1.0099" y2="6.6599" layer="51"/>
<rectangle x1="-2.8001" y1="6.21" x2="-2.2799" y2="6.6599" layer="51"/>
<rectangle x1="-4.0701" y1="6.21" x2="-3.5499" y2="6.6599" layer="51"/>
<rectangle x1="-5.3401" y1="6.21" x2="-4.8199" y2="6.6599" layer="51"/>
<rectangle x1="-7.9299" y1="3.5499" x2="-7.48" y2="4.0701" layer="51"/>
<rectangle x1="-7.9299" y1="2.2799" x2="-7.48" y2="2.8001" layer="51"/>
<rectangle x1="-7.9299" y1="1.0099" x2="-7.48" y2="1.5301" layer="51"/>
<rectangle x1="-1" y1="-1" x2="1" y2="1" layer="35"/>
</package>
<package name="TSOP28">
<description>&lt;b&gt;Thin Small Outline Package Gull Wing&lt;/b&gt;&lt;p&gt;
type I, package type TS</description>
<wire x1="-4" y1="-8.8" x2="-4" y2="8.8" width="0.254" layer="21"/>
<wire x1="-4" y1="8.8" x2="4" y2="8.8" width="0.254" layer="21"/>
<wire x1="4" y1="8.8" x2="4" y2="-8.8" width="0.254" layer="21"/>
<wire x1="4" y1="-8.8" x2="-4" y2="-8.8" width="0.254" layer="21"/>
<smd name="1" x="-3.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="2" x="-3.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="3" x="-2.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="4" x="-2.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="5" x="-1.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="6" x="-1.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="7" x="-0.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="8" x="0.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="9" x="1.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="10" x="1.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="11" x="2.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="12" x="2.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="13" x="3.25" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="14" x="3.75" y="-9.7" dx="0.3" dy="1" layer="1"/>
<smd name="15" x="3.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="16" x="3.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="17" x="2.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="18" x="2.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="19" x="1.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="20" x="1.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="21" x="0.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="22" x="-0.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="23" x="-1.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="24" x="-1.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="25" x="-2.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="26" x="-2.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="27" x="-3.25" y="9.7" dx="0.3" dy="1" layer="1"/>
<smd name="28" x="-3.75" y="9.7" dx="0.3" dy="1" layer="1"/>
<text x="-2.75" y="-7.4" size="1.016" layer="21" rot="R180">1</text>
<text x="-4.286" y="-8.8021" size="1.27" layer="25" rot="R90">&gt;NAME</text>
<text x="1.476" y="-7.2781" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
<rectangle x1="-3.89" y1="-10" x2="-3.61" y2="-8.88" layer="51"/>
<rectangle x1="-3.39" y1="-10" x2="-3.11" y2="-8.88" layer="51"/>
<rectangle x1="-2.89" y1="-10" x2="-2.61" y2="-8.88" layer="51"/>
<rectangle x1="-2.39" y1="-10" x2="-2.11" y2="-8.88" layer="51"/>
<rectangle x1="-1.89" y1="-10" x2="-1.61" y2="-8.88" layer="51"/>
<rectangle x1="-1.39" y1="-10" x2="-1.11" y2="-8.88" layer="51"/>
<rectangle x1="-0.89" y1="-10" x2="-0.61" y2="-8.88" layer="51"/>
<rectangle x1="0.61" y1="-10" x2="0.89" y2="-8.88" layer="51"/>
<rectangle x1="1.11" y1="-10" x2="1.39" y2="-8.88" layer="51"/>
<rectangle x1="1.61" y1="-10" x2="1.89" y2="-8.88" layer="51"/>
<rectangle x1="2.11" y1="-10" x2="2.39" y2="-8.88" layer="51"/>
<rectangle x1="2.61" y1="-10" x2="2.89" y2="-8.88" layer="51"/>
<rectangle x1="3.11" y1="-10" x2="3.39" y2="-8.88" layer="51"/>
<rectangle x1="3.61" y1="-10" x2="3.89" y2="-8.88" layer="51"/>
<rectangle x1="3.61" y1="8.88" x2="3.89" y2="10" layer="51"/>
<rectangle x1="3.11" y1="8.88" x2="3.39" y2="10" layer="51"/>
<rectangle x1="2.61" y1="8.88" x2="2.89" y2="10" layer="51"/>
<rectangle x1="2.11" y1="8.88" x2="2.39" y2="10" layer="51"/>
<rectangle x1="1.61" y1="8.88" x2="1.89" y2="10" layer="51"/>
<rectangle x1="1.11" y1="8.88" x2="1.39" y2="10" layer="51"/>
<rectangle x1="0.61" y1="8.88" x2="0.89" y2="10" layer="51"/>
<rectangle x1="-0.89" y1="8.88" x2="-0.61" y2="10" layer="51"/>
<rectangle x1="-1.39" y1="8.88" x2="-1.11" y2="10" layer="51"/>
<rectangle x1="-1.89" y1="8.88" x2="-1.61" y2="10" layer="51"/>
<rectangle x1="-2.39" y1="8.88" x2="-2.11" y2="10" layer="51"/>
<rectangle x1="-2.89" y1="8.88" x2="-2.61" y2="10" layer="51"/>
<rectangle x1="-3.39" y1="8.88" x2="-3.11" y2="10" layer="51"/>
<rectangle x1="-3.89" y1="8.88" x2="-3.61" y2="10" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="27C128">
<wire x1="-7.62" y1="30.48" x2="10.16" y2="30.48" width="0.254" layer="94"/>
<wire x1="10.16" y1="30.48" x2="10.16" y2="-33.02" width="0.254" layer="94"/>
<wire x1="10.16" y1="-33.02" x2="-7.62" y2="-33.02" width="0.254" layer="94"/>
<wire x1="-7.62" y1="-33.02" x2="-7.62" y2="30.48" width="0.254" layer="94"/>
<text x="-7.62" y="31.75" size="1.778" layer="95">&gt;NAME</text>
<text x="-7.62" y="-35.56" size="1.778" layer="96">&gt;VALUE</text>
<pin name="A0" x="-10.16" y="7.62" length="short" direction="in"/>
<pin name="A1" x="-10.16" y="5.08" length="short" direction="in"/>
<pin name="A2" x="-10.16" y="2.54" length="short" direction="in"/>
<pin name="A3" x="-10.16" y="0" length="short" direction="in"/>
<pin name="A4" x="-10.16" y="-2.54" length="short" direction="in"/>
<pin name="A5" x="-10.16" y="-5.08" length="short" direction="in"/>
<pin name="A6" x="-10.16" y="-7.62" length="short" direction="in"/>
<pin name="A7" x="-10.16" y="-10.16" length="short" direction="in"/>
<pin name="A8" x="-10.16" y="-12.7" length="short" direction="in"/>
<pin name="!CE" x="-10.16" y="12.7" length="short" direction="in"/>
<pin name="!OE" x="-10.16" y="15.24" length="short" direction="in"/>
<pin name="D0" x="12.7" y="7.62" length="short" direction="hiz" rot="R180"/>
<pin name="D1" x="12.7" y="5.08" length="short" direction="hiz" rot="R180"/>
<pin name="D2" x="12.7" y="2.54" length="short" direction="hiz" rot="R180"/>
<pin name="D3" x="12.7" y="0" length="short" direction="hiz" rot="R180"/>
<pin name="D4" x="12.7" y="-2.54" length="short" direction="hiz" rot="R180"/>
<pin name="D5" x="12.7" y="-5.08" length="short" direction="hiz" rot="R180"/>
<pin name="D6" x="12.7" y="-7.62" length="short" direction="hiz" rot="R180"/>
<pin name="D7" x="12.7" y="-10.16" length="short" direction="hiz" rot="R180"/>
<pin name="VCC" x="-10.16" y="27.94" length="short" direction="pwr"/>
<pin name="GND" x="-10.16" y="-30.48" length="short" direction="pwr"/>
<pin name="A9" x="-10.16" y="-15.24" length="short" direction="in"/>
<pin name="A10" x="-10.16" y="-17.78" length="short" direction="in"/>
<pin name="!PGM" x="-10.16" y="20.32" length="short" direction="pas"/>
<pin name="A11" x="-10.16" y="-20.32" length="short" direction="in"/>
<pin name="A12" x="-10.16" y="-22.86" length="short" direction="in"/>
<pin name="VPP" x="-10.16" y="25.4" length="short" direction="in"/>
<pin name="A13" x="-10.16" y="-25.4" length="short" direction="in"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="27*128" prefix="IC">
<description>CMOS &lt;B&gt;EPROM&lt;/B&gt;&lt;p&gt;
128 K (16 K x 8)</description>
<gates>
<gate name="G$1" symbol="27C128" x="0" y="0"/>
</gates>
<devices>
<device name="P" package="DIL28-6">
<connects>
<connect gate="G$1" pin="!CE" pad="20"/>
<connect gate="G$1" pin="!OE" pad="22"/>
<connect gate="G$1" pin="!PGM" pad="27"/>
<connect gate="G$1" pin="A0" pad="10"/>
<connect gate="G$1" pin="A1" pad="9"/>
<connect gate="G$1" pin="A10" pad="21"/>
<connect gate="G$1" pin="A11" pad="23"/>
<connect gate="G$1" pin="A12" pad="2"/>
<connect gate="G$1" pin="A13" pad="26"/>
<connect gate="G$1" pin="A2" pad="8"/>
<connect gate="G$1" pin="A3" pad="7"/>
<connect gate="G$1" pin="A4" pad="6"/>
<connect gate="G$1" pin="A5" pad="5"/>
<connect gate="G$1" pin="A6" pad="4"/>
<connect gate="G$1" pin="A7" pad="3"/>
<connect gate="G$1" pin="A8" pad="25"/>
<connect gate="G$1" pin="A9" pad="24"/>
<connect gate="G$1" pin="D0" pad="11"/>
<connect gate="G$1" pin="D1" pad="12"/>
<connect gate="G$1" pin="D2" pad="13"/>
<connect gate="G$1" pin="D3" pad="15"/>
<connect gate="G$1" pin="D4" pad="16"/>
<connect gate="G$1" pin="D5" pad="17"/>
<connect gate="G$1" pin="D6" pad="18"/>
<connect gate="G$1" pin="D7" pad="19"/>
<connect gate="G$1" pin="GND" pad="14"/>
<connect gate="G$1" pin="VCC" pad="28"/>
<connect gate="G$1" pin="VPP" pad="1"/>
</connects>
<technologies>
<technology name="C">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="L" package="PLCC-32">
<connects>
<connect gate="G$1" pin="!CE" pad="23"/>
<connect gate="G$1" pin="!OE" pad="25"/>
<connect gate="G$1" pin="!PGM" pad="31"/>
<connect gate="G$1" pin="A0" pad="11"/>
<connect gate="G$1" pin="A1" pad="10"/>
<connect gate="G$1" pin="A10" pad="24"/>
<connect gate="G$1" pin="A11" pad="27"/>
<connect gate="G$1" pin="A12" pad="3"/>
<connect gate="G$1" pin="A13" pad="30"/>
<connect gate="G$1" pin="A2" pad="9"/>
<connect gate="G$1" pin="A3" pad="8"/>
<connect gate="G$1" pin="A4" pad="7"/>
<connect gate="G$1" pin="A5" pad="6"/>
<connect gate="G$1" pin="A6" pad="5"/>
<connect gate="G$1" pin="A7" pad="4"/>
<connect gate="G$1" pin="A8" pad="29"/>
<connect gate="G$1" pin="A9" pad="28"/>
<connect gate="G$1" pin="D0" pad="13"/>
<connect gate="G$1" pin="D1" pad="14"/>
<connect gate="G$1" pin="D2" pad="15"/>
<connect gate="G$1" pin="D3" pad="18"/>
<connect gate="G$1" pin="D4" pad="19"/>
<connect gate="G$1" pin="D5" pad="20"/>
<connect gate="G$1" pin="D6" pad="21"/>
<connect gate="G$1" pin="D7" pad="22"/>
<connect gate="G$1" pin="GND" pad="16"/>
<connect gate="G$1" pin="VCC" pad="32"/>
<connect gate="G$1" pin="VPP" pad="2"/>
</connects>
<technologies>
<technology name="C">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
<device name="TS" package="TSOP28">
<connects>
<connect gate="G$1" pin="!CE" pad="27"/>
<connect gate="G$1" pin="!OE" pad="1"/>
<connect gate="G$1" pin="!PGM" pad="6"/>
<connect gate="G$1" pin="A0" pad="17"/>
<connect gate="G$1" pin="A1" pad="16"/>
<connect gate="G$1" pin="A10" pad="28"/>
<connect gate="G$1" pin="A11" pad="2"/>
<connect gate="G$1" pin="A12" pad="9"/>
<connect gate="G$1" pin="A13" pad="5"/>
<connect gate="G$1" pin="A2" pad="15"/>
<connect gate="G$1" pin="A3" pad="14"/>
<connect gate="G$1" pin="A4" pad="13"/>
<connect gate="G$1" pin="A5" pad="12"/>
<connect gate="G$1" pin="A6" pad="11"/>
<connect gate="G$1" pin="A7" pad="10"/>
<connect gate="G$1" pin="A8" pad="4"/>
<connect gate="G$1" pin="A9" pad="3"/>
<connect gate="G$1" pin="D0" pad="18"/>
<connect gate="G$1" pin="D1" pad="19"/>
<connect gate="G$1" pin="D2" pad="20"/>
<connect gate="G$1" pin="D3" pad="22"/>
<connect gate="G$1" pin="D4" pad="23"/>
<connect gate="G$1" pin="D5" pad="24"/>
<connect gate="G$1" pin="D6" pad="25"/>
<connect gate="G$1" pin="D7" pad="26"/>
<connect gate="G$1" pin="GND" pad="21"/>
<connect gate="G$1" pin="VCC" pad="7"/>
<connect gate="G$1" pin="VPP" pad="8"/>
</connects>
<technologies>
<technology name="C">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
<technology name="LV">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="TOP" library="con-subd" deviceset="M25" device="H"/>
<part name="BOTTOM" library="con-subd" deviceset="M25" device="H"/>
<part name="SRAM" library="ic-package" deviceset="DIL32" device=""/>
<part name="IC1" library="microchip" deviceset="27*128" device="P" technology="C"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="TOP" gate="-1" x="2.54" y="78.74"/>
<instance part="BOTTOM" gate="-1" x="2.54" y="30.48"/>
<instance part="SRAM" gate="G$1" x="86.36" y="33.02"/>
<instance part="IC1" gate="G$1" x="40.64" y="27.94"/>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="1"/>
<wire x1="10.16" y1="91.44" x2="15.24" y2="91.44" width="0.1524" layer="91"/>
<label x="10.16" y="91.44" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="VPP"/>
<wire x1="30.48" y1="53.34" x2="27.94" y2="53.34" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="16"/>
<wire x1="78.74" y1="12.7" x2="73.66" y2="12.7" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="22"/>
<wire x1="93.98" y1="25.4" x2="99.06" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="+5" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="2"/>
<wire x1="10.16" y1="88.9" x2="15.24" y2="88.9" width="0.1524" layer="91"/>
<label x="10.16" y="88.9" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="VCC"/>
<wire x1="30.48" y1="55.88" x2="27.94" y2="55.88" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="2"/>
<wire x1="78.74" y1="48.26" x2="73.66" y2="48.26" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="32"/>
<wire x1="93.98" y1="50.8" x2="99.06" y2="50.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="+12" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="3"/>
<wire x1="10.16" y1="86.36" x2="15.24" y2="86.36" width="0.1524" layer="91"/>
<label x="10.16" y="86.36" size="1.778" layer="95"/>
</segment>
</net>
<net name="/IRQ0" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="4"/>
<wire x1="10.16" y1="83.82" x2="15.24" y2="83.82" width="0.1524" layer="91"/>
<label x="10.16" y="83.82" size="1.778" layer="95"/>
</segment>
</net>
<net name="CLK" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="14"/>
<wire x1="-5.08" y1="91.44" x2="-10.16" y2="91.44" width="0.1524" layer="91"/>
<label x="-5.08" y="91.44" size="1.778" layer="95" rot="MR0"/>
</segment>
</net>
<net name="-5" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="15"/>
<wire x1="-5.08" y1="88.9" x2="-10.16" y2="88.9" width="0.1524" layer="91"/>
<label x="-5.08" y="88.9" size="1.778" layer="95" rot="MR0"/>
</segment>
</net>
<net name="-12" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="16"/>
<wire x1="-5.08" y1="86.36" x2="-10.16" y2="86.36" width="0.1524" layer="91"/>
<label x="-5.08" y="86.36" size="1.778" layer="95" rot="MR0"/>
</segment>
</net>
<net name="/IRQ1" class="0">
<segment>
<pinref part="TOP" gate="-1" pin="17"/>
<wire x1="-5.08" y1="83.82" x2="-10.16" y2="83.82" width="0.1524" layer="91"/>
<label x="-5.08" y="83.82" size="1.778" layer="95" rot="MR0"/>
</segment>
</net>
<net name="D7" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="13"/>
<wire x1="10.16" y1="12.7" x2="15.24" y2="12.7" width="0.1524" layer="91"/>
<label x="10.16" y="12.7" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D7"/>
<wire x1="53.34" y1="17.78" x2="58.42" y2="17.78" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="21"/>
<wire x1="93.98" y1="22.86" x2="99.06" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D5" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="12"/>
<wire x1="10.16" y1="15.24" x2="15.24" y2="15.24" width="0.1524" layer="91"/>
<label x="10.16" y="15.24" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D5"/>
<wire x1="53.34" y1="22.86" x2="58.42" y2="22.86" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="19"/>
<wire x1="93.98" y1="17.78" x2="99.06" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D3" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="11"/>
<wire x1="10.16" y1="17.78" x2="15.24" y2="17.78" width="0.1524" layer="91"/>
<label x="10.16" y="17.78" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D3"/>
<wire x1="53.34" y1="27.94" x2="58.42" y2="27.94" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="17"/>
<wire x1="93.98" y1="12.7" x2="99.06" y2="12.7" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D1" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="10"/>
<wire x1="10.16" y1="20.32" x2="15.24" y2="20.32" width="0.1524" layer="91"/>
<label x="10.16" y="20.32" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D1"/>
<wire x1="53.34" y1="33.02" x2="58.42" y2="33.02" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="14"/>
<wire x1="78.74" y1="17.78" x2="73.66" y2="17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D6" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="25"/>
<wire x1="-5.08" y1="15.24" x2="-10.16" y2="15.24" width="0.1524" layer="91"/>
<label x="-5.08" y="15.24" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D6"/>
<wire x1="53.34" y1="20.32" x2="58.42" y2="20.32" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="20"/>
<wire x1="93.98" y1="20.32" x2="99.06" y2="20.32" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D4" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="24"/>
<wire x1="-5.08" y1="17.78" x2="-10.16" y2="17.78" width="0.1524" layer="91"/>
<label x="-5.08" y="17.78" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D4"/>
<wire x1="53.34" y1="25.4" x2="58.42" y2="25.4" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="18"/>
<wire x1="93.98" y1="15.24" x2="99.06" y2="15.24" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D2" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="23"/>
<wire x1="-5.08" y1="20.32" x2="-10.16" y2="20.32" width="0.1524" layer="91"/>
<label x="-5.08" y="20.32" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D2"/>
<wire x1="53.34" y1="30.48" x2="58.42" y2="30.48" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="15"/>
<wire x1="78.74" y1="15.24" x2="73.66" y2="15.24" width="0.1524" layer="91"/>
</segment>
</net>
<net name="D0" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="22"/>
<wire x1="-5.08" y1="22.86" x2="-10.16" y2="22.86" width="0.1524" layer="91"/>
<label x="-5.08" y="22.86" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="D0"/>
<wire x1="53.34" y1="35.56" x2="58.42" y2="35.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="13"/>
<wire x1="78.74" y1="20.32" x2="73.66" y2="20.32" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A0" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="1"/>
<wire x1="10.16" y1="43.18" x2="15.24" y2="43.18" width="0.1524" layer="91"/>
<label x="10.16" y="43.18" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A0"/>
<wire x1="30.48" y1="35.56" x2="27.94" y2="35.56" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="12"/>
<wire x1="78.74" y1="22.86" x2="73.66" y2="22.86" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A2" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="2"/>
<wire x1="10.16" y1="40.64" x2="15.24" y2="40.64" width="0.1524" layer="91"/>
<label x="10.16" y="40.64" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A2"/>
<wire x1="30.48" y1="30.48" x2="27.94" y2="30.48" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="10"/>
<wire x1="78.74" y1="27.94" x2="73.66" y2="27.94" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A4" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="3"/>
<wire x1="10.16" y1="38.1" x2="15.24" y2="38.1" width="0.1524" layer="91"/>
<label x="10.16" y="38.1" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A4"/>
<wire x1="30.48" y1="25.4" x2="27.94" y2="25.4" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="8"/>
<wire x1="78.74" y1="33.02" x2="73.66" y2="33.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A6" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="4"/>
<wire x1="10.16" y1="35.56" x2="15.24" y2="35.56" width="0.1524" layer="91"/>
<label x="10.16" y="35.56" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A6"/>
<wire x1="30.48" y1="20.32" x2="27.94" y2="20.32" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="6"/>
<wire x1="78.74" y1="38.1" x2="73.66" y2="38.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A8" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="5"/>
<wire x1="10.16" y1="33.02" x2="15.24" y2="33.02" width="0.1524" layer="91"/>
<label x="10.16" y="33.02" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A8"/>
<wire x1="30.48" y1="15.24" x2="27.94" y2="15.24" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="27"/>
<wire x1="93.98" y1="38.1" x2="99.06" y2="38.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A10" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="6"/>
<wire x1="10.16" y1="30.48" x2="15.24" y2="30.48" width="0.1524" layer="91"/>
<label x="10.16" y="30.48" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A10"/>
<wire x1="30.48" y1="10.16" x2="27.94" y2="10.16" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="23"/>
<wire x1="93.98" y1="27.94" x2="99.06" y2="27.94" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A12" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="7"/>
<wire x1="10.16" y1="27.94" x2="15.24" y2="27.94" width="0.1524" layer="91"/>
<label x="10.16" y="27.94" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A12"/>
<wire x1="30.48" y1="5.08" x2="27.94" y2="5.08" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="4"/>
<wire x1="78.74" y1="43.18" x2="73.66" y2="43.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A14" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="8"/>
<wire x1="10.16" y1="25.4" x2="15.24" y2="25.4" width="0.1524" layer="91"/>
<label x="10.16" y="25.4" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="3"/>
<wire x1="78.74" y1="45.72" x2="73.66" y2="45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A1" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="14"/>
<wire x1="-5.08" y1="43.18" x2="-10.16" y2="43.18" width="0.1524" layer="91"/>
<label x="-5.08" y="43.18" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A1"/>
<wire x1="30.48" y1="33.02" x2="27.94" y2="33.02" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="11"/>
<wire x1="78.74" y1="25.4" x2="73.66" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A3" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="15"/>
<wire x1="-5.08" y1="40.64" x2="-10.16" y2="40.64" width="0.1524" layer="91"/>
<label x="-5.08" y="40.64" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A3"/>
<wire x1="30.48" y1="27.94" x2="27.94" y2="27.94" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="9"/>
<wire x1="78.74" y1="30.48" x2="73.66" y2="30.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A5" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="16"/>
<wire x1="-5.08" y1="38.1" x2="-10.16" y2="38.1" width="0.1524" layer="91"/>
<label x="-5.08" y="38.1" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A5"/>
<wire x1="30.48" y1="22.86" x2="27.94" y2="22.86" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="7"/>
<wire x1="78.74" y1="35.56" x2="73.66" y2="35.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A7" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="17"/>
<wire x1="-5.08" y1="35.56" x2="-10.16" y2="35.56" width="0.1524" layer="91"/>
<label x="-5.08" y="35.56" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A7"/>
<wire x1="30.48" y1="17.78" x2="27.94" y2="17.78" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="5"/>
<wire x1="78.74" y1="40.64" x2="73.66" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A9" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="18"/>
<wire x1="-5.08" y1="33.02" x2="-10.16" y2="33.02" width="0.1524" layer="91"/>
<label x="-5.08" y="33.02" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A9"/>
<wire x1="30.48" y1="12.7" x2="27.94" y2="12.7" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="26"/>
<wire x1="93.98" y1="35.56" x2="99.06" y2="35.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A11" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="19"/>
<wire x1="-5.08" y1="30.48" x2="-10.16" y2="30.48" width="0.1524" layer="91"/>
<label x="-5.08" y="30.48" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A11"/>
<wire x1="30.48" y1="7.62" x2="27.94" y2="7.62" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="25"/>
<wire x1="93.98" y1="33.02" x2="99.06" y2="33.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A13" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="20"/>
<wire x1="-5.08" y1="27.94" x2="-10.16" y2="27.94" width="0.1524" layer="91"/>
<label x="-5.08" y="27.94" size="1.778" layer="95" rot="MR0"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="A13"/>
<wire x1="30.48" y1="2.54" x2="27.94" y2="2.54" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SRAM" gate="G$1" pin="28"/>
<wire x1="93.98" y1="40.64" x2="99.06" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="A15" class="0">
<segment>
<pinref part="BOTTOM" gate="-1" pin="21"/>
<wire x1="-5.08" y1="25.4" x2="-10.16" y2="25.4" width="0.1524" layer="91"/>
<label x="-5.08" y="25.4" size="1.778" layer="95" rot="MR0"/>
</segment>
</net>
<net name="DECODE_A15" class="0">
<segment>
<pinref part="SRAM" gate="G$1" pin="31"/>
<wire x1="93.98" y1="48.26" x2="99.06" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SRAM_CS2" class="0">
<segment>
<pinref part="SRAM" gate="G$1" pin="30"/>
<wire x1="93.98" y1="45.72" x2="99.06" y2="45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="/MW" class="0">
<segment>
<pinref part="SRAM" gate="G$1" pin="29"/>
<wire x1="93.98" y1="43.18" x2="99.06" y2="43.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="/MR" class="0">
<segment>
<pinref part="SRAM" gate="G$1" pin="24"/>
<wire x1="93.98" y1="30.48" x2="99.06" y2="30.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="SRAM" gate="G$1" pin="1"/>
<wire x1="78.74" y1="50.8" x2="73.66" y2="50.8" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
