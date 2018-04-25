.PHONY:

ifndef OUT
OUT=./copgccf
endif

CMP=g++ -std=gnu++11
WARN=-Wall -Wno-unused-result -Wno-deprecated-declarations -Wno-sign-compare -Wno-maybe-uninitialized
OPTIM=-Ofast -march=native -funroll-loops -funsafe-loop-optimizations -falign-functions=16 -falign-loops=16 -fopenmp
NOOPTIM=-O0 -march=native -fopenmp
DBG=-g ${NOOPTIM}

COBJSUBDIR=cobj
DEPSUBDIR=dep

ECHOCC=echo "[\033[01;33m CC \033[0m]"
ECHOLD=echo "[\033[01;36m LD \033[0m]"

OPT=${OPTIM}

define compilec
${ECHOCC} $(notdir $<) ;\
mkdir -p ${DEPSUBDIR} ;\
tmp=`mktemp` ;\
${CMP} ${INC} -MM ${OPT} $< >> $$tmp ;\
if [ $$? -eq 0 ] ;\
then echo -n "${COBJSUBDIR}/" > ${DEPSUBDIR}/$(notdir $<).d ;\
cat $$tmp >> ${DEPSUBDIR}/$(notdir $<).d ;\
rm $$tmp ;\
mkdir -p ${COBJSUBDIR} ;\
cd ${COBJSUBDIR} ;\
${CMP} -c ${INC} ${OPT} ${WARN} ../$< ;\
else \
ret=$$? ;\
rm $$tmp ;\
exit $$ret ;\
fi
endef

all: copgccf

-include ${DEPSUBDIR}/*.d

copgccf: ${COBJSUBDIR}/copgccf.o ${COBJSUBDIR}/io.o ${COBJSUBDIR}/random.o ${COBJSUBDIR}/induced.o ${COBJSUBDIR}/pseudotree.o ${COBJSUBDIR}/ui.o ${COBJSUBDIR}/dslyce.o ${COBJSUBDIR}/req.o
	@${ECHOLD} copgccf
	@${CMP} ${OPT} -Lui $^ -lnetstream -o ${OUT}

${COBJSUBDIR}/io.o: io.cpp
	@$(compilec)

${COBJSUBDIR}/random.o: random.cpp
	@$(compilec)

${COBJSUBDIR}/induced.o: induced.cpp
	@$(compilec)

${COBJSUBDIR}/req.o: req.cpp
	@$(compilec)

${COBJSUBDIR}/ui.o: ui.cpp
	@$(compilec)

${COBJSUBDIR}/pseudotree.o: pseudotree.cpp
	@$(compilec)

${COBJSUBDIR}/dslyce.o: dslyce.cpp
	@$(compilec)

${COBJSUBDIR}/copgccf.o: copgccf.cpp
	@$(compilec)

clean:
	@echo "Removing subdirectories..."
	@rm -rf ${COBJSUBDIR} ${DEPSUBDIR}

run:
	./copgccf 4 test.wcsp test.order

rundebug:
	gdb ${OUT}
