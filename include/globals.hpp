#ifndef CLOAK_GLOBALS_HPP
#define CLOAK_GLOBALS_HPP

#include "defines.hpp"
#include "extern.hpp"
#include "uci_options.hpp"
#include "board.hpp"
#include <string>
#include <shared_mutex>

const std::string logger_name("Logger");
const std::string engine_name("Prometheus");
const std::string version("0.1");
const std::string author("Zach Bortoff");

const unsigned int MAX_DEPTH = 20;
const unsigned int MIN_DEPTH = 2;

const Centipawns_t INF = 1000000;

const uint8_t CASTLE_CHECK[64] {
    13,15,15,15,12,15,15,14,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
     7,15,15,15, 3,15,15,11
};

const Bitboard ANTI_DIAGONAL_MASK[15] {
    C64(0x80),
    C64(0x8040),
    C64(0x804020),
    C64(0x80402010),
    C64(0x8040201008),
    C64(0x804020100804),
    C64(0x80402010080402),
    C64(0x8040201008040201),
    C64(0x4020100804020100),
    C64(0x2010080402010000),
    C64(0x1008040201000000),
    C64(0x804020100000000),
    C64(0x402010000000000),
    C64(0x201000000000000),
    C64(0x100000000000000)
};

const Bitboard DIAGONAL_MASK[15] {
    C64(0x1),
    C64(0x102),
    C64(0x10204),
    C64(0x1020408),
    C64(0x102040810),
    C64(0x10204081020),
    C64(0x1020408102040),
    C64(0x102040810204080),
    C64(0x204081020408000),
    C64(0x408102040800000),
    C64(0x810204080000000),
    C64(0x1020408000000000),
    C64(0x2040800000000000),
    C64(0x4080000000000000),
    C64(0x8000000000000000)
};

const Bitboard RANK_MASK[8] {
    C64(0xFF),
    C64(0xFF00),
    C64(0xFF0000),
    C64(0xFF000000),
    C64(0xFF00000000),
    C64(0xFF0000000000),
    C64(0xFF000000000000),
    C64(0xFF00000000000000)
};

const Bitboard FILE_MASK[8] {
    C64(0x101010101010101),
    C64(0x202020202020202),
    C64(0x404040404040404),
    C64(0x808080808080808),
    C64(0x1010101010101010),
    C64(0x2020202020202020),
    C64(0x4040404040404040),
    C64(0x8080808080808080)
};

const Bitboard KING_ATTACK_ARRAY[64] {
    C64(770), C64(1797), C64(3594), C64(7188),
    C64(14376), C64(28752), C64(57504), C64(49216),
    C64(197123), C64(460039), C64(920078),C64(1840156),
    C64(3680312), C64(7360624), C64(14721248), C64(12599488),
    C64(50463488), C64(117769984), C64(235539968),
    C64(471079936), C64(942159872), C64(1884319744),
    C64(3768639488), C64(3225468928), C64(12918652928),
    C64(30149115904), C64(60298231808), C64(120596463616),
    C64(241192927232), C64(482385854464), C64(964771708928),
    C64(825720045568), C64(3307175149568), C64(7718173671424),
    C64(15436347342848), C64(30872694685696), C64(61745389371392),
    C64(123490778742784), C64(246981557485568), C64(211384331665408),
    C64(846636838289408), C64(1975852459884544), C64(3951704919769088),
    C64(7903409839538176), C64(15806819679076352), C64(31613639358152704),
    C64(63227278716305408), C64(54114388906344448), C64(216739030602088448),
    C64(505818229730443264), C64(1011636459460886528),
    C64(2023272918921773056), C64(4046545837843546112),
    C64(8093091675687092224), C64(16186183351374184448),
    C64(13853283560024178688), C64(144959613005987840),
    C64(362258295026614272), C64(724516590053228544),
    C64(1449033180106457088), C64(2898066360212914176),
    C64(5796132720425828352), C64(11592265440851656704),
    C64(4665729213955833856)
};

const Bitboard KNIGHT_ATTACK_ARRAY[64] {
    C64(132096), C64(329728), C64(659712), C64(1319424),
    C64(2638848), C64(5277696), C64(10489856), C64(4202496),
    C64(33816580), C64(84410376), C64(168886289),
    C64(337772578), C64(675545156), C64(1351090312),
    C64(2685403152), C64(1075839008), C64(8657044482),
    C64(21609056261), C64(43234889994), C64(86469779988),
    C64(172939559976), C64(345879119952), C64(687463207072),
    C64(275414786112), C64(2216203387392), C64(5531918402816),
    C64(11068131838464), C64(22136263676928), C64(44272527353856),
    C64(88545054707712), C64(175990581010432), C64(70506185244672),
    C64(567348067172352), C64(1416171111120896), C64(2833441750646784),
    C64(5666883501293568), C64(11333767002587136), C64(22667534005174272),
    C64(45053588738670592), C64(18049583422636032), C64(145241105196122112),
    C64(362539804446949376), C64(725361088165576704),
    C64(1450722176331153408), C64(2901444352662306816),
    C64(5802888705324613632), C64(11533718717099671552),
    C64(4620693356194824192), C64(288234782788157440),
    C64(576469569871282176), C64(1224997833292120064),
    C64(2449995666584240128), C64(4899991333168480256),
    C64(9799982666336960512), C64(1152939783987658752),
    C64(2305878468463689728), C64(1128098930098176), C64(2257297371824128),
    C64(4796069720358912), C64(9592139440717824), C64(19184278881435648),
    C64(38368557762871296), C64(4679521487814656), C64(9077567998918656)
};

const PositionKey POSITION_KEYS[12][64] {
    {
        C64(8105683865553257089), C64(5775475781758098805),
        C64(11295652867192264170), C64(7824107849183854172),
        C64(13350022388915595203), C64(6077962761174578426),
        C64(8567980898453674604), C64(396459502541227478),
        C64(14934200219069087879), C64(15957279614222586271),
        C64(7786111669621098812), C64(9790680585706113649),
        C64(9133948377631519812), C64(11293953202287642268),
        C64(1420383812189966456), C64(18158851994905421136),
        C64(15917411965471766380), C64(13725465181954154888),
        C64(1139384150682617511), C64(39134450436742120),
        C64(7665675558664251674), C64(17254181465765155501),
        C64(11309062523265602569), C64(6065142799769375435),
        C64(6352447487822588625), C64(6346856004073071057),
        C64(3814749999684714642), C64(18001301708131104350),
        C64(12085580312981782553), C64(14033175415385856800),
        C64(5084370642799232499), C64(13088742906312215440),
        C64(17036667968707051475), C64(15730163202781475377),
        C64(1426122720899631754), C64(6640741756126943225),
        C64(6407519836411804804), C64(3102721457268420757),
        C64(1966749231900441075), C64(5482764698979201550),
        C64(14242979901322858585), C64(6805645733986243534),
        C64(6624720541948581247), C64(8847156671367411409),
        C64(11836130274831863030), C64(9894118083352551555),
        C64(7071366799125259081), C64(5680057373664873824),
        C64(2938502341004721207), C64(14451454345861831095),
        C64(3016088220071986669), C64(10166695315121853494),
        C64(10231421027036029145), C64(916779803179823526),
        C64(7111328027132680852), C64(11638946922523340391),
        C64(17763954611187113426), C64(696450557438321637),
        C64(13991714405561537013), C64(2633326493301248236),
        C64(141221641552645030), C64(3358324167533930902),
        C64(9966299804792791280), C64(8719687882228302907)
    },
    {
        C64(13121375828665745976), C64(11230161796394321581),
        C64(4492836933778203765), C64(13317964698340515546),
        C64(11583735113068049325), C64(5513443179966138292),
        C64(17385568615298723179), C64(8006484697551687500),
        C64(5180151321049614496), C64(11935560394115024027),
        C64(1502298743792149121), C64(8236936865805620866),
        C64(18444800213431345800), C64(5646727756571416726),
        C64(17041558931419829195), C64(13636054483172608479),
        C64(4389551273537494800), C64(17719289136062471631),
        C64(7236884311780589726), C64(3515763871491829198),
        C64(6239478431033985205), C64(10318916677808827535),
        C64(4393816942183513428), C64(5959528921838639912),
        C64(11068398583436118165), C64(2980057290480413724),
        C64(4373482206133197532), C64(6833532173191256487),
        C64(957954535536225355), C64(12387850371526288797),
        C64(4343563538827016864), C64(13503128560026306026),
        C64(13608600206279300009), C64(15460996979286279838),
        C64(8689941374437520367), C64(9773715098167616363),
        C64(919622400779277054), C64(9046898918109041426),
        C64(2583844562329234553), C64(1308762475539227542),
        C64(10244185118097909281), C64(14545412206135667852),
        C64(15641394490622438808), C64(4355130690384990136),
        C64(14400642985673583168), C64(12262449442981847211),
        C64(14788577578753809893), C64(8952804790064447056),
        C64(8964524235441644834), C64(2023728017913189339),
        C64(2970723196106672045), C64(2606423290793403581),
        C64(13661840138721282247), C64(7260867034345069451),
        C64(14579420700004280670), C64(3456191367070666130),
        C64(5844378848018461974), C64(15166144871556752555),
        C64(2236058248053646289), C64(15193765671627581169),
        C64(12934050465322769429), C64(9655205279774676796),
        C64(13642867973425343162), C64(3635115552478250068)
    },
    {
        C64(1351939343417132228), C64(716865345664846157),
        C64(211384269432683306), C64(6362643820753945189),
        C64(9115896803615092884), C64(7429874302597804570),
        C64(8452379392744560343), C64(10643328567912468106),
        C64(5238971516939493442), C64(14214826724109014966),
        C64(8891094907652575650), C64(1588040909603202483),
        C64(9498172658156869955), C64(961524360258241904),
        C64(2798237466265035277), C64(9027794318013928046),
        C64(13109553955853170134), C64(15656537777089540647),
        C64(8526270541251011111), C64(8479068089098141477),
        C64(6114004255890319441), C64(9053402993130658924),
        C64(17977891265571660080), C64(584011374200850283),
        C64(16958187926441821385), C64(15276583572773921227),
        C64(10585294566501330093), C64(17734567083960233961),
        C64(7446522216175162856), C64(6118405109937017108),
        C64(1001082233507125130), C64(13449451839339104880),
        C64(2418857690454422506), C64(11866782461197383594),
        C64(7386919286865808340), C64(7715192592955872132),
        C64(2680275202995173189), C64(12163896520210389215),
        C64(10519430427707168303), C64(9737731219499998222),
        C64(15656836430309481897), C64(11147790337599596864),
        C64(14625088717094291936), C64(235485706498453742),
        C64(5830352345476335769), C64(154182846171019596),
        C64(7556187903288580742), C64(4500028258138616487),
        C64(1247273053033045910), C64(17741903003656815696),
        C64(11693821902174816053), C64(13482741878745327424),
        C64(5468765570387154792), C64(14171051807600801067),
        C64(9477126898879831526), C64(15332487033725706531),
        C64(18157078352078279290), C64(10816965995605620172),
        C64(9676097346473378226), C64(6579984062497975769),
        C64(4258277830146147120), C64(562613000380305562),
        C64(3962740092545660427), C64(11226012234898764491)
    },
    {
        C64(6544450376437561936), C64(14293397967852302006),
        C64(14185543455607591078), C64(12553893256981342524),
        C64(6264854431279453197), C64(5732781836659938452),
        C64(9713374789758088907), C64(13069322201848914398),
        C64(15739963036390895360), C64(15656023691060159194),
        C64(6129518953823356306), C64(10578131136052624845),
        C64(5861726483947084614), C64(6782859488784260555),
        C64(16104594419161648379), C64(9653833435633085329),
        C64(2190925631769073419), C64(16012399501581846467),
        C64(15779809285291755524), C64(18302183605852341550),
        C64(7265735824170007598), C64(5597390173250694485),
        C64(10826997743934444433), C64(2780732401058758210),
        C64(10604012787469446176), C64(17339081527783702775),
        C64(11367353813308805757), C64(16548965905859323681),
        C64(2286342185040676186), C64(8584924686428541584),
        C64(11688768479463664537), C64(11614800382652623862),
        C64(15339549563970967573), C64(1805388583588875249),
        C64(16361409933884542333), C64(16712113031901737710),
        C64(1292606254717147666), C64(533468184121898902),
        C64(8750045742550378252), C64(17676531280437732444),
        C64(8454614047019222985), C64(18340780241004610384),
        C64(2289527026050108379), C64(15524258221332833060),
        C64(6896585354305734932), C64(6397336466015539358),
        C64(7738930564873518436), C64(10949299329716436815),
        C64(16888453763496309126), C64(14923696072153906351),
        C64(7154955514404471700), C64(715142735897346518),
        C64(4481146980517322518), C64(804663692231878100),
        C64(7517606877738347881), C64(6952043908090181808),
        C64(8297589658061698853), C64(800168475520101634),
        C64(3279893100138598907), C64(2477338110724069854),
        C64(403839659415981047), C64(13278807457746346006),
        C64(10277381772304564990), C64(10221723708695401106)
    },
    {
        C64(6793919506869883130), C64(14442130727803134906),
        C64(4037474572194300611), C64(4502368203897221123),
        C64(16109948115258067862), C64(14363916466511900571),
        C64(3772927427246345658), C64(15892979698849684902),
        C64(2510019505233883350), C64(916052282473464639),
        C64(12760047940073360369), C64(15284576863917164144),
        C64(13351579548748799351), C64(9609099105628195470),
        C64(10092903739481938319), C64(4081330376298952020),
        C64(3903279814925187719), C64(1035242875907877157),
        C64(8655979559955930383), C64(16325488872856478579),
        C64(5173931061009227541), C64(6759780080483214241),
        C64(1570298532563061032), C64(14341295481175857260),
        C64(10062112271914768974), C64(6226727582774854866),
        C64(3319761899775808983), C64(17162522891425209954),
        C64(18051188726083578435), C64(15531098734974641156),
        C64(684941661620017051), C64(5772839336715809418),
        C64(16339647264220017848), C64(7640086664316917073),
        C64(8867794905403918981), C64(14048105081648142650),
        C64(2613263717426425808), C64(2199106023385332949),
        C64(5244585617295291810), C64(14336122082805027143),
        C64(3735750088353002502), C64(3124332462218236406),
        C64(10433358182041640156), C64(15574651161613935514),
        C64(191589549283310086), C64(16240190924246051291),
        C64(5661449965813262719), C64(7463043136653220789),
        C64(4009782114538397640), C64(4916659586290416616),
        C64(15665863650144124993), C64(5083407677593631799),
        C64(10632133309042743262), C64(8758947213420231550),
        C64(6096470725371044423), C64(5287304628898527014),
        C64(17604359960833290548), C64(619035833555364306),
        C64(3049628531782034674), C64(7696995327756083984),
        C64(12611944289841330750), C64(14376326295802812907),
        C64(10205216739284080493), C64(18015234610806040340)
    },
    {
        C64(393095330188340290), C64(7886466245326118854),
        C64(13151274133623054371), C64(7088944793708468178),
        C64(15044244039519911254), C64(10234981966445694616),
        C64(14785206588714615622), C64(1665136842128888213),
        C64(15122279667010658804), C64(2005426718355564238),
        C64(12360531743962602948), C64(5428501396165529096),
        C64(17063295824068518290), C64(11934284491556732479),
        C64(8133431939812350347), C64(15989271050465425145),
        C64(8580503323959285107), C64(504541389365059961),
        C64(11288333545859021609), C64(2925553698523456900),
        C64(6134265153817261475), C64(18194433753231685706),
        C64(3670634285067490252), C64(17586181422404634177),
        C64(12444596034698015680), C64(3314015772169652552),
        C64(7124647019837136829), C64(6411580090063163382),
        C64(17722801883751870936), C64(8393943244075836008),
        C64(5190335831799865934), C64(2785359201802061324),
        C64(14098317047901284128), C64(14661267733827408493),
        C64(15251346989109399455), C64(11220951475986528853),
        C64(8011967278911040587), C64(10861695424910507374),
        C64(4060902595848751566), C64(13523818931894272210),
        C64(13802861157021397777), C64(6974016041861930885),
        C64(7271441562100085676), C64(5020569009211839290),
        C64(15366363230358967624), C64(11491748279996009981),
        C64(12164660151211770428), C64(18183693474118751018),
        C64(17914442310448895076), C64(13139389034512926441),
        C64(6630606123657455637), C64(18120171903444795099),
        C64(11033805521753691064), C64(3490430760884720983),
        C64(3117900461027436803), C64(14767956324776854410),
        C64(1216356187273562212), C64(11280367864963840838),
        C64(16131134212847762906), C64(3619316905614553503),
        C64(10549778478438594628), C64(3319822224533134732),
        C64(14813825046611675765), C64(13235843670028602102)
    },
    {
        C64(11034178526913512837), C64(9807330132663597400),
        C64(7048699499364319619), C64(6615531684049859058),
        C64(3390677116444477337), C64(13489219593829126083),
        C64(3513491574122492264), C64(9441663160859652159),
        C64(4538023012919910550), C64(15819524116913761926),
        C64(18149868915341111040), C64(5786925618907891555),
        C64(4561119691632602424), C64(10879160498748174588),
        C64(706795399508970157), C64(6417184042940267779),
        C64(11785232858370648722), C64(5214867241082992493),
        C64(17463658673350772173), C64(7576698316202877874),
        C64(12888290627869161926), C64(9238675037143642527),
        C64(16811758043816857555), C64(17020746379705328591),
        C64(10040331822287394118), C64(9183954466239220734),
        C64(1551071022779605815), C64(9607468270883750193),
        C64(9954101543454817517), C64(17556459361624371458),
        C64(8907168704715184321), C64(9776240890167398289),
        C64(2131436608398134640), C64(17973374367777447415),
        C64(17836476861333263667), C64(6255570554036072974),
        C64(3261174968375526731), C64(10085760410814950922),
        C64(11649525083270217895), C64(1177008776808942617),
        C64(2480393692372696211), C64(8035229483386616040),
        C64(15812679722219211977), C64(10213210064986783032),
        C64(18332188362277428318), C64(16580919682347194965),
        C64(6369298059680655342), C64(4686959114764882645),
        C64(15151953923829294311), C64(1913160747828938966),
        C64(4435597773626102245), C64(5204810919639647424),
        C64(28450621935360031), C64(2832198254016994367),
        C64(6622853540344931477), C64(16857680632951574338),
        C64(16614674333733999303), C64(5549762261928204031),
        C64(61911377126432171), C64(7523489433501586990),
        C64(9177031431819049695), C64(1976087171372413987),
        C64(3760239991044204016), C64(7160119012419539961)
    },
    {
        C64(15018027970192663641), C64(13214007362451309614),
        C64(17113067295005929367), C64(1929720703751659707),
        C64(8142919978201490740), C64(4103761945666894438),
        C64(9607784771095305473), C64(5390727833555743993),
        C64(9441322272933616032), C64(663779443453630638),
        C64(15757034270451977151), C64(12641718961267284364),
        C64(16912672287155278028), C64(17670287981001167044),
        C64(5130675273393598708), C64(14598497671454858505),
        C64(7758364805812769393), C64(11739799640462359093),
        C64(14171614745204106375), C64(7320501477221432235),
        C64(6376842663104709856), C64(18203123093147301876),
        C64(16714755960103183105), C64(3239420237948856929),
        C64(89952991130755211), C64(11554589924487952196),
        C64(9230355740566662691), C64(17479457184343447702),
        C64(7607414917552823365), C64(16778498791644456283),
        C64(14437614436743452710), C64(12456635830827424820),
        C64(17640564941365519827), C64(11669367873100401364),
        C64(688883633863031927), C64(2543904654260940309),
        C64(1799435480823104927), C64(3185397931732742999),
        C64(8188209428689335381), C64(7032396073821621408),
        C64(13843784726374782459), C64(18147297452666737947),
        C64(2322243567460806962), C64(16478740508775707759),
        C64(87864789059958808), C64(1303676226066350241),
        C64(3491449815565579201), C64(6743172902949609958),
        C64(3013306900158037433), C64(5775222220048018555),
        C64(18110991358314740564), C64(1652367800770329107),
        C64(16338597523689983532), C64(15369017032772612880),
        C64(8717019897380538110), C64(6747400215806330562),
        C64(467088041424812604), C64(3366060974434510623),
        C64(7033012248859364431), C64(1468665577572740866),
        C64(12942930147549860881), C64(2851090513410280476),
        C64(11416279826259599244), C64(85754227381099881)
    },
    {
        C64(15967054895611185929), C64(11284294360361774411),
        C64(13020083810610988192), C64(18390269678236829343),
        C64(1701747040700182783), C64(7138870324679419259),
        C64(18042919365096540165), C64(15080591694232907765),
        C64(8769358430111860308), C64(4714439811282360374),
        C64(16830343340792689974), C64(4236438816336490006),
        C64(14020357278788670942), C64(18363959236245307295),
        C64(11897660277096345184), C64(4576393203575661860),
        C64(2737816923502524610), C64(7525687710213972389),
        C64(15510994344679295975), C64(6834181461542557788),
        C64(11052727981383712452), C64(6588738430212510526),
        C64(1943159366662840116), C64(17438310279740993071),
        C64(9725773610371972130), C64(5899199127260446435),
        C64(14516308279083658148), C64(4850060799880001203),
        C64(11145214496059536143), C64(1383893130278283410),
        C64(904468507058339158), C64(10623120701777789206),
        C64(17820988444416274530), C64(11246301076559754811),
        C64(17255302617881441076), C64(12755492220567592670),
        C64(17600233337145894537), C64(1696063862244387572),
        C64(12998579215008212715), C64(15077740906362550046),
        C64(13499815264925777533), C64(2323522389988663526),
        C64(3326450623723860507), C64(6769078449373177214),
        C64(14201025482600034998), C64(12746770246672189096),
        C64(6145528273945787170), C64(6499962912723648026),
        C64(16988651563371416549), C64(12702639330697181576),
        C64(17868281899026702539), C64(10232170287572109476),
        C64(15247724578316676561), C64(7084355593561281821),
        C64(16943613732146635703), C64(14693431687766344730),
        C64(11113300772318242982), C64(17270594530451522455),
        C64(2444901939524289323), C64(17889408802333357176),
        C64(17475206342854522229), C64(3509108664511970663),
        C64(12018710474327830557), C64(5763993712722062348)
    },
    {
        C64(5519178666483630403), C64(16246769207410502789),
        C64(9940797370045883794), C64(16294460203803494078),
        C64(16531562201057863744), C64(5416262215968942065),
        C64(11838058548610965906), C64(13985349346338688947),
        C64(14002952923180258082), C64(13843989541848143692),
        C64(9989337045894055748), C64(10209708616989552592),
        C64(11493468855117277538), C64(6167335412375760724),
        C64(16554018951518864617), C64(11993142046134758960),
        C64(699515994439470327), C64(2000520002692455279),
        C64(11458208634057711541), C64(8053281007282691055),
        C64(6881669273036819105), C64(11177502198593046898),
        C64(10078958209068867467), C64(5197995224773137233),
        C64(14884940091284351682), C64(5468211583527830642),
        C64(619567171506403033), C64(14574172221219816757),
        C64(5818690077025802804), C64(802292433470976755),
        C64(11769511900547273904), C64(16749966915950950650),
        C64(4326912837849448236), C64(12466249214100280847),
        C64(6310274367905511940), C64(265887172173746014),
        C64(15615023504360582306), C64(6259163230113184975),
        C64(16877962866905705441), C64(14026773855197014801),
        C64(15870151777515015336), C64(5862981428192603093),
        C64(7525776578092408838), C64(1562697489242521254),
        C64(10911048991569577332), C64(11822042742353968359),
        C64(12020738246049970243), C64(18047596135783450452),
        C64(11635949421502181424), C64(18168502256887043620),
        C64(13669968484287668639), C64(167558814139259779),
        C64(17414180807456557145), C64(17649551043143096029),
        C64(385603034358865943), C64(9726680656967283116),
        C64(5244169519285659029), C64(11374048964785395803),
        C64(12615163322182730370), C64(1691533216203439575),
        C64(8585329379292530720), C64(10593433455297477824),
        C64(12916746042603131646), C64(6041081990481061046)
    },
    {
        C64(2082104266781506117), C64(6155285915129481650),
        C64(13947561611654669165), C64(4118432703633407149),
        C64(6692054469585592727), C64(3348756830514031253),
        C64(16828344941054633387), C64(6067285174147025375),
        C64(8280310263686139769), C64(4718977454504264719),
        C64(9003260181228599893), C64(4305544882111833828),
        C64(11279155052990153345), C64(2875716368422252334),
        C64(13074224100836136472), C64(8631990759102623872),
        C64(11404580341588891341), C64(5557559965952836709),
        C64(2478599903540912093), C64(2987478796411048661),
        C64(6071666320485997628), C64(6968258568388049445),
        C64(8549454452724685979), C64(7126180565153065563),
        C64(6872172965874230343), C64(16753398013386207325),
        C64(13298783085325965492), C64(14663702714303177280),
        C64(16981432205316588343), C64(2987435992159731754),
        C64(8284203999476899124), C64(9017522105105600581),
        C64(9062461501017361066), C64(11520949253498248127),
        C64(12761934031480234087), C64(1378804447641790787),
        C64(10220091662454945935), C64(9965887495417779017),
        C64(4694820844797839937), C64(13249116918394941172),
        C64(12330020682544069494), C64(14800112835930634338),
        C64(12072474418201135137), C64(6002417338873763924),
        C64(3171440022429413739), C64(8064550966535684733),
        C64(500225343731879792), C64(2314740995444832112),
        C64(17943405322915510009), C64(4991032041874292473),
        C64(1268467241203042781), C64(14461338388873917129),
        C64(1262626046229344006), C64(8347272955070686949),
        C64(11525622190543606794), C64(9042009024554227863),
        C64(12403064331637995495), C64(9136961116072993656),
        C64(12639718876935007606), C64(2553008989301055698),
        C64(2956275595906717004), C64(3667561043775224721),
        C64(7653033448306101055), C64(14311228022475432535)
    },
    {
        C64(5647857363559545894), C64(114107586579848089),
        C64(13467291785943392467), C64(4786991864593380939),
        C64(15585112089291385618), C64(15539650333940576616),
        C64(1013456314367178974), C64(7019989777906316490),
        C64(5968699259447246616), C64(6523163642405001183),
        C64(17326346091837655819), C64(7522049585008984875),
        C64(4998579047244273582), C64(4017640531919196580),
        C64(7360467427860347062), C64(13671892939497429815),
        C64(6911506521972795417), C64(15287141615706522655),
        C64(13054296999322754244), C64(13971808753658730005),
        C64(18288851322305697463), C64(7130723694864107481),
        C64(6400404678751744970), C64(10813620323311212397),
        C64(6446467287735086671), C64(17902061959817540836),
        C64(236146210842422705), C64(17660030860425165809),
        C64(16936750587577464299), C64(13324534151180507609),
        C64(6558243797674558422), C64(11847286478744978763),
        C64(11897553790469783660), C64(16611760108813544938),
        C64(708766735314290437), C64(5451115897025380340),
        C64(15954064324026595804), C64(14748315714111322969),
        C64(8827276871854215287), C64(6750058519842991872),
        C64(4300361517055089430), C64(3108811476005635404),
        C64(11749760856318451246), C64(2508923611929064266),
        C64(15107604922362605297), C64(17981647096414818941),
        C64(14161013403720705007), C64(12252905960819417292),
        C64(2490455708051005604), C64(1386401856328167102),
        C64(176467890992390247), C64(12757241535948257626),
        C64(4255790217208035757), C64(13259994464145706440),
        C64(13402568014551059222), C64(11115054944891584477),
        C64(12741114520322398878), C64(16764482388873979081),
        C64(15493910478265576247), C64(10905943843456894164),
        C64(17945510577213694991), C64(565323901234341026),
        C64(5818554335253132953), C64(7191311788278210979)
    }
};
const PositionKey SIDE_2_MOVE_KEYS[2] {
    C64(6175141700388318186), C64(8056736653868951741)
};
const PositionKey CASTLE_PERMISSION_KEYS[16] {
    C64(1524010862746082182), C64(14477572640875699494),
    C64(18236447223257024037), C64(5724811918033203271),
    C64(3094956986382331529), C64(16948614797807040585),
    C64(5072451344660835905), C64(10939911114567274264),
    C64(4413273379127765240), C64(6068823696507229268),
    C64(1042994794936569136), C64(1066536446361863805),
    C64(4025655915269392334), C64(5137895415035378653),
    C64(17366195441973466958), C64(11781942122580254476)
};
const PositionKey EN_PASSANT_KEYS[17] {
    C64(7601756374976232775), C64(6372158061490466337),
    C64(44021279151823199), C64(11552071463189921649),
    C64(9023231063320267338), C64(16160949136853682967),
    C64(2655825186664565244), C64(1264846735125213974),
    C64(7440875441430186093), C64(16471116484232382430),
    C64(15240051677347303755), C64(4295134354464884561),
    C64(7559569801333248209), C64(2682090975941783820),
    C64(5471588771802347397), C64(17745574171694516151),
    C64(11995557675011424376)
};

/// Evaluation Parameters
const Centipawns_t PAWN_BASE_VAL   =  100;
const Centipawns_t BISHOP_BASE_VAL =  325; // 325 -> 370
const Centipawns_t KNIGHT_BASE_VAL =  325; // 325 -> 310
const Centipawns_t ROOK_BASE_VAL   =  500;
const Centipawns_t QUEEN_BASE_VAL  =  1000; // 910 ->1000

const Centipawns_t TEMPO = 5;

const double QUEEN_PERC[4] = {
    0.9, 0.95, 1.2, 1.3
};
const double ROOK_PERC[4] = {
    0.85, 0.9, 1.1, 1.1
};
const double BISHOP_PERC[4] = {
    1.0, 1.05, 1.15, 1.15
};
const double KNIGHT_PERC[4] = {
    1.1, 1.1, 0.9, 0.85
};
const double PAWN_STRENGTH[4] = {
    1.0, 1.5, 1.1, 1.15
};
const double CONNECTIVITY[5] = {
    9.1, 5.0, 3.3, 3.1, 1.0
};

const Centipawns_t KNIGHT_MOBILITY = 10;
const Centipawns_t KNIGHT_OUTPOST_BONUS = 5;
const Centipawns_t KNIGHT_PST[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   6,  12,  15,  15,  12,   6, -30,
    -40, -20,   1,   1,   1,   1, -20, -40,
    -50, -30, -30, -30, -30, -30, -40, -50
};

const Centipawns_t OM_ROOK_MOBILITY = 10;
const Centipawns_t E_ROOK_MOBILITY = 10;
const Centipawns_t ROOK_ON_HALF_OPEN_FILE_BONUS = 8;
const Centipawns_t ROOK_ON_OPEN_FILE_BONUS = 15;
const Centipawns_t ROOK_ON_OPEN_FILE_BONUS_NO_OPPOSING_ROOK = 25;
const Centipawns_t ROOK_SUPPORTING_PASSED_PAWNS = 50;
const Centipawns_t CONNECTED_ROOK_BONUS = 30;
const Centipawns_t ROOK_PST[64] = {
    -40, -20,  3, 25, 25,  3, -20, -40,
    -20,   0,  0, 15, 15,  0,   0, -20,
    -10,   0,  0, 10, 10,  0,   0, -10,
     -5,   0,  0,  5,  5,  0,   0,  -5,
     -5,   0,  0,  5,  5,  0,   0,  -5,
     -10,  0,  0,  5,  5,  0,   0, -10,
     10,  10, 10, 40, 40, 10,  10,  10,
    -40, -20,  0, 20, 20,  0, -20, -40
};
const Centipawns_t ROOK_ON_SEVENTH = 9;

const Centipawns_t OM_BISHOP_MOBILITY = 10;
const Centipawns_t BISHOP_PST[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,  10,   0,   0,   0,   0,  10, -10,
    -10,   4,  15,  10,  10,  15,   4, -10,
    -10,  -5,  10,  20,  20,  10,  -5, -10,
    -10,   5,  10,  20,  20,  10,   5, -10,
    -10,  -5,  15,  10,  10,  15,  -5, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -40, -10, -10, -40, -10, -20
};
const Centipawns_t BISHOP_PAIR_BONUS = 10;
const Centipawns_t BISHOP_ON_LONG_DIAGONAL = 7;
const Centipawns_t BISHOP_ON_OPEN_LONG_DIAGONAL = 12;

const Centipawns_t QUEEN_MOBILITY = 10;
const Centipawns_t QUEEN_PST[64] = {
    -50, -30, -10, -5, -5, -10, -30, -50,
    -30,   0,  -5, 10, 10,  -5,   0, -30,
    -10,  -9,  15, 20, 20,  15,  -9, -10,
     -5,   5,  17, 30, 30,  17,   5,  -5,
     -5,   5,  17, 30, 30,  17,   5,  -5,
    -10,  -9,  15, 15, 15,  15,  -9, -10,
     10,  10,  10, 10, 10,  10,  10,  10,
    -50, -30, -10, -5, -5, -10, -30, -50 // -50, -30, -10, -5, -5, -10, -30, -50
};

const Centipawns_t OM_KING_MOBILITY = 11;
const Centipawns_t E_KING_MOBILITY = 15;
const Centipawns_t M_KING_PST[64] = {
     20,  15,   5,  -10,  -10,   5,  15,  20,
    -20, -20, -20,  -20,  -20, -20, -20, -20,
    -10, -10, -20,  -20,  -20, -20, -20, -10,
    -30, -30, -30, -100, -100, -30, -30, -30,
    -40, -40, -40, -100, -100, -40, -40, -30,
    -30, -40, -40,  -50,  -50, -40, -40, -30,
    -30, -40, -40,  -50,  -50, -40, -40, -30,
    -30, -40, -40,  -50,  -50, -40, -40, -30
};

const Centipawns_t E_KING_PST[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

const Centipawns_t PAWN_MOBILITY = 10;
const Centipawns_t PAWN_PST[64] = {
    0,   0,  0,    0,   0,   0,  0,  0,
    5,   0,  10, -20, -20,  10,  0,  5,
    5,  -5, -10,  20,  20, -10, -5,  5,
    0,   0,  20,  40,  40,  20,  0,  0,
    5,   5,  20,  40,  40,  20,  5,  5,
    10, 10,  20,  30,  30,  20, 10, 10,
    50, 50,  50,  50,  50,  50, 50, 50,
    0,   0,   0,   0,   0,   0,  0,  0
};

const Centipawns_t PASSED_PAWN[8] = {
    //0, 0, 0, 12, 25, 50, 100, 0 // make these weights MUCH bigger
    0, 0, 0, 15, 30, 90, 900, 0
};

const Centipawns_t PAWN_ATTACK_CENTER = 15;
const Centipawns_t PAWN_UNOPPOSED_PAWN = 5;
const Centipawns_t PAWN_BACKWARD = -5;
const Centipawns_t PAWN_BLOCKING = -5;
const Centipawns_t PAWN_DOUBLE = -25; // penalty for each pawn that's doubled. so if 3 are doubled, then 75
const Centipawns_t PAWN_ISOLATED = -20;
const Centipawns_t PAWN_KING_SHELTER = 10; // BONUS FOR COMPOSING KING SHELTER
const Centipawns_t PAWN_IN_CHAIN = 10; // BONUS FOR BEING IN PAWN CHAIN https://www.chessprogramming.org/Defended_Pawns_(Bitboards)
const Centipawns_t PAWN_AS_BASE_OF_CHAIN = 5; // BONUS FOR ATTACKING / DEFENDING BASE
const Centipawns_t PAWN_AS_HEAD_OF_CHAIN = -2;
const Centipawns_t PAWN_ATTACKING_ENEMY_CAMP = 20;
const Centipawns_t PAWN_HANGING = -15;
const Centipawns_t PAWN_ISLAND[4] = {
    0, 5, 10, 15
};
const Centipawns_t HALF_ISOLATED_PAWN = -5;

/// opening (development)
// move same piece more than once (-20 repeat piece -10 repeat pawn)
// early exchange of queens with loss of castling rights = -40
const Centipawns_t Q_B4_2_MINORS = -90;
const Centipawns_t OVERALL_DEV_BONUS = 175;

/// Search Parameters
const Centipawns_t WINDOW_SIZE[3] = {
    150, 550, 1100
};
const Depth NULL_MOVE_R = 2; // 3
const Depth FUTILITY_DEPTH = 2;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_1 = 500;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_2 = 600;
const Centipawns_t FUTILITY_THRESHOLD_DEPTH_3 = 850;
const Depth MULTICUT_R = 3;
const Depth MULTICUT_DEPTH = 3;
const Depth MULTICUT_M = 15;
const Depth MULTICUT_C = 5;
const Depth CHECKOUT_EXTENSION = 2;
const Depth ONE_REPLY_EXTENSION = 2;
const Depth RECAPTURE_EXTENSION = 2;
const Depth PASSED_PAWN_EXTENSION_SEVENTH = 2;
const Depth MATE_THREAT_EXTENSION = 2;
const Depth LATE_MOVE_REDUCTION_PLY_DISCOUNT = 1;
const Depth LATE_MOVE_REDUCTION_DISCOUNT_NUMBER = 6;
const double LATE_MOVE_REDUCTION_DISCOUNT_PERCENTAGE = 0.25;
const int AGE_DIFFERENCE_THRESHOLD = 16;

#endif // CLOAK_GLOBALS_HPP
