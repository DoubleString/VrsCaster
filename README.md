# VrsCaster
#Caster based on rtklib 2.4.3,including self-defined vrs mode
#Configures:
#log:logfile and its update interval(hour)
#sourcetable: the path of the sourcetable file
#casterc-path: connection information of ntrip-client usr:psd@:port
#casters-path: connection information of ntrip-server psd@:port
#caster-vrslist: the vrs service server,if no server is pointed,make it empty
#caster-vrsinfo: the mountpoint correspoinding with the vrs service connection,which will be automatically added to the sourcetable

#samples of confitures:
{
"caster":{
	"log":["log.txt","0.5"],
	"sourcetable":"sourcetable",
	"casterc-path":"root:123@:2103",
	"casters-path":":123@:2104",
	"caster-vrslist":[],
	"caster-vrsinfo":[]
	}
}
## under this configures,ntrip client can use root:123@ip:2103/mnt to acquire network stream
##                       nitrp server can use :123@:2104 to update network stream

#content of sourcetable:
STR;MOUNTPOINT;VRS;RTCM 3.2;1;2;GPS+GLO+BDS;SGNET;CHN;30.52;114.31;0;1;BAMBOO;None;B;N;0;
