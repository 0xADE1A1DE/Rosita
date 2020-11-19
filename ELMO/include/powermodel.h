/*
 * University of Bristol – Open Access Software Licence
 * Copyright (c) 2016, The University of Bristol, a chartered
 * corporation having Royal Charter number RC000648 and a charity
 * (number X1121) and its place of administration being at Senate
 * House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Any use of the software for scientific publications or commercial
 * purposes should be reported to the University of Bristol
 * (OSI-notifications@bristol.ac.uk and quote reference 2668). This is
 * for impact and usage monitoring purposes only.
 *
 * Enquiries about further applications and development opportunities
 * are welcome. Please contact elisabeth.oswald@bristol.ac.uk
 */
/* 
 * University of Adelaide
 * Copyright (c) 2020
 */
#include <libgen.h>

int hweight(unsigned int n)
{
    unsigned int c; // the total bits set in n
    for (c = 0; n; n >>= 1)
    {
        c += n & 1;
    }
    return c;
}

//-------------------------------------------------------------------

int hdistance(unsigned int x, unsigned int y)
{
    int dist = 0;
    unsigned  val = x ^ y;
    // Count the number of bits set
    while (val != 0)
    {
        // A bit is set, so increment the count and clear the bit
        dist++;
        val &= val - 1;
    }

    // Return the number of differing bits
    return dist;
}

#ifdef POWERMODEL_HW

//-------------------------------------------------------------------
void hwpowermodel(){

    char str[500], filepath[500];
    FILE *fp, *fp_nonprofiled;
    double differentialvoltage, supplycurrent, power;

    int hw_op1, hw_op2, hd_op1, hd_op2, instructiontype, i, j, count, index = 1;
    double PrvInstr_data = 0, SubInstr_data = 0, Operand1_data = 0, Operand2_data = 0, BitFlip1_data = 0, BitFlip2_data = 0, HWOp1PrvInstr_data = 0, HWOp2PrvInstr_data = 0, HDOp1PrvInstr_data = 0, HDOp2PrvInstr_data = 0, HWOp1SubInstr_data = 0, HWOp2SubInstr_data = 0, HDOp1SubInstr_data = 0, HDOp2SubInstr_data = 0, Operand1_bitinteractions_data = 0, Operand2_bitinteractions_data = 0, BitFlip1_bitinteractions_data = 0, BitFlip2_bitinteractions_data = 0;

    previous = start;
    current = start->next;
    subsequent = start->next->next;

    strcpy(str, TRACEFOLDER);
    strcat(str, TRACEFILE);
    sprintf(filepath, str, t);
    fp = fopen(filepath, "w+");

    if(t==1 || PRINTALLNONPROFILEDTRACES){
        strcpy(str, NONPROFILEDFOLDER);
        strcat(str, NONPROFILEDFILE);
        sprintf(filepath, str, t);
        fp_nonprofiled = fopen(filepath, "w+");
    }

    while(subsequent->next != NULL){

        instructiontype = current->instruction_typedec;

        // Test for key guessing space
        // if(t == 1)
        //  keyflowfailtest(current);

        // Instruction was not profiled

        if(instructiontype == 5){
            dataptr->instruction_type[0] = 1;
            instructiontype = 0;

            if(t==1 || PRINTALLNONPROFILEDTRACES)
                fprintf(fp_nonprofiled,"%d %x\n",index, dataptr->pc);
        }
        else{
            hw_op1 = hweight(current->op1);
            hw_op2 = hweight(current->op2);

            hd_op1 = hdistance(previous->op1, current->op1);
            hd_op2 = hdistance(previous->op2, current->op2);
        }

        power = (double)hw_op2;

        if(instructiontype == 2 | instructiontype == 3){
            if(CYCLEACCURATE){
#ifdef BINARYTRACES
                fwrite(&power, sizeof(power), 1, fp);
                fwrite(&power, sizeof(power), 1, fp);
#else
                fprintf(fp,"%0.40f\n",power);
                fprintf(fp,"%0.40f\n",power);
#endif
                index += 2;
            }
            else{
#ifdef BINARYTRACES
                fwrite(&power, sizeof(power), 1, fp);
#else
                fprintf(fp,"%0.40f\n",power);
#endif
                index += 1;
            }
        }
        else{
#ifdef BINARYTRACES
            fwrite(&power, sizeof(power), 1, fp);
#else
            fprintf(fp,"%0.40f\n",power);
#endif
            index += 1;
        }

    previous = previous->next;
    current = current->next;
    subsequent = subsequent->next;

}

fclose(fp);

if(t==1 || PRINTALLNONPROFILEDTRACES) fclose(fp_nonprofiled);

}

#else

#define NINST 4000
#define NPROP 26
#define TTESTIDX 25
//-------------------------------------------------------------------
// SET TOTAL TRACES BEFORE RUNNING ELMO!
// 
int totaltraces = 40000;
int totalinsts = 0;
struct TVar
{
    //double K, Ex, Ex2;
    //int n;
    int count;
    double mean;
    double M2;
};

struct TVar tvars[2][NPROP][NINST];
int inst_type[NINST];
double ttestvalues[NINST];
void TVar_init(struct TVar* tv)
{
    tv->count = 0;
    tv->mean = 0.0;
    tv->M2 = 0.0;
}
void set_inst_type(int type, int index)
{
    if (index >= NINST)
    {
        printf("Inst buffer not enough\n");
        exit(-1);
    }
    inst_type[index] = type;
}

void add_variable(struct TVar* tv,double x)
{
   /*
    if (tv->n == 0)
    {
        tv->K = x;
    }

    double y = x - tv->K;
    tv->n = tv->n + 1;
    tv->Ex += y;
    tv->Ex2 += y*y;*/
   
    tv->count +=1;
    double delta = x - tv->mean;
    tv->mean += delta / tv->count;
    double delta2 = x - tv->mean;
    tv->M2 += delta * delta2;
    //printf("%d\n",tv->n);
}
double get_meanvalue(struct TVar* tv)
{
    /*double d = tv->K + tv->Ex / tv->n;
    if (isnan(d))
    {
        printf("isnan hit %lf %lf %d\n",tv->K , tv->Ex , tv->n);
        exit(-1);
    }
    return d;*/
    return tv->mean;
}
double get_variance(struct TVar* tv)
{
    //return tv->Ex2;
    //double h = (tv->Ex);

    //printf("v %lf %lf %lf \n",tv->Ex2 , (h*h)/tv->n, (tv->n-1));
    //return (tv->Ex2 - (h*h)/tv->n) / (tv->n-1);
    if (tv->count < 2)
    {
        printf("nan");
        exit(-1);
    }
    return tv->M2/tv->count;
}
void calc_props_ini(char *exec_file)
{
    char* exec_file_cp = strdup(exec_file);
    char* exec_dir = dirname(exec_file_cp);
    char tracecount_buf[256];
    snprintf(tracecount_buf, 255, "%s/tracecount.txt", exec_dir);
    free(exec_file_cp);
    FILE *ft = fopen(tracecount_buf, "r");
    if (!ft)
    {
        printf("place tracecount.txt with traces count in exec dir\n");
        exit(-1);
    }
    
    fscanf(ft,"%d",&totaltraces);
    printf("%d\n",totaltraces);
    fclose(ft);
    
    int i=0;
    for (;i<NPROP;i++)
    {
        int j=0;
        for (;j<NINST;j++)
        {
            /*tvars[0][i][j].K=0.0;
            tvars[0][i][j].n=0;
            tvars[0][i][j].Ex=0.0;
            tvars[0][i][j].Ex2=0.0;
            tvars[1][i][j].K=0.0;
            tvars[1][i][j].n=0;
            tvars[1][i][j].Ex=0.0;
            tvars[1][i][j].Ex2=0.0;*/
            
            TVar_init(&tvars[0][i][j]);
            TVar_init(&tvars[1][i][j]);
        }
    }
}
void calc_props(double prop, int prop_id, int index)
{
    if (index >= NINST)
    {
        printf("Not enough space in instruction buffer\n");
        exit(-1);
    }
    int NT = totaltraces/2;
    struct TVar* tv = &tvars[(t-1)/NT][prop_id][index];
    add_variable(tv, prop);
    if (totalinsts < index+1)
    {
        totalinsts = index+1;
    }

}
void calc_props_fin()
{
    
    int NT = totaltraces/2;
    FILE* f = fopen("./output/fixedvsrandompropttest.txt","w");
    int j=0;
    double tvals[NPROP];

    for (;j<totalinsts;j++)
    {
        int i=0;
        for (;i<NPROP;i++)
        {
            struct TVar* tv1 = &tvars[0][i][j];
            struct TVar* tv2 = &tvars[1][i][j];
            //printf("%0.20lf %0.20lf\n",get_meanvalue(tv1),get_meanvalue(tv2));
            double a = sqrt(NT)*(get_meanvalue(tv1)- get_meanvalue(tv2));
            //printf("%lf %d %d\n",a,i,j);
            double b = sqrt(get_variance(tv1) + get_variance(tv2));
            //printf("sqrt %0.20lf\n", b);
            if (b != 0.0)
            {
                a /= b;
            }
            else if (a != 0.0)
            {
                // means are different, while the respective variances are 0
                a = 987654321;
            }
            if (isnan(a))
            {
                printf("nan %lf\n", b);
            }
            tvals[i] = a;
            if (TTESTIDX == i)
            {
                ttestvalues[j] = a;
            }
        }

        if (inst_type[j] == 2 || inst_type[j] == 3)
        {
            if(CYCLEACCURATE)
            {
                i=0;
                for (;i<NPROP;i++)
                    fprintf(f,"%lf ",tvals[i]);
                fprintf(f, "\n");
            }
        }
        i=0;
        for (;i<NPROP;i++)
            fprintf(f,"%lf ",tvals[i]);

        fprintf(f, "\n");
    }
    fclose(f);
}
void calc_props_dump_tvals()
{
    int i=0;
    FILE *fp = fopen("./output/fixedvsrandomtstatistics.txt","w");
    for (;i<totalinsts;i++)
    {
        fprintf(fp, "%lf\n", ttestvalues[i]);
    }
    fclose(fp);
}
#define GET_BIT(_b, _x) \
    ((_x >> _b) & 1U)
void elmopowermodel(){
    // NM_ prefixed variables are not included in model because they are not used for leakage detection
    // they used to detect the kind of leakage (model already includes these effects)
    char str[500], filepath[500];
    FILE *fp, *fp_nonprofiled;
    double differentialvoltage, supplycurrent, power;
    int last_ldrstr_state=-1;
    int hw_op1, hw_op2, hd_op1, hd_op2, instructiontype, i, j, count, index = 1;
    double PrvInstr_data = 0, SubInstr_data = 0, Operand1_data = 0, Operand2_data = 0, BitFlip1_data = 0, BitFlip2_data = 0, HWOp1PrvInstr_data = 0, HWOp2PrvInstr_data = 0, HDOp1PrvInstr_data = 0, HDOp2PrvInstr_data = 0, HWOp1SubInstr_data = 0, HWOp2SubInstr_data = 0, HDOp1SubInstr_data = 0, HDOp2SubInstr_data = 0, Operand1_bitinteractions_data = 0, Operand2_bitinteractions_data = 0, BitFlip1_bitinteractions_data = 0, BitFlip2_bitinteractions_data = 0;
    double Op1Op2_Overwrite_data = 0;
    double NM_Op1Op2_Overwrite_data = 0;
    double NM_Op1_Overwrite_data = 0;
    double NM_Op2_Overwrite_data = 0;
    double State_data = 0;
    double state_cum_data[STATE_CLASS_COUNT] = {0};
    uint32_t states[STATE_CLASS_COUNT] = {0};
    uint16_t states_op1_reg[STATE_CLASS_COUNT] = {ST_REG_NONE};
    uint16_t states_op2_reg[STATE_CLASS_COUNT] = {ST_REG_NONE};
    uint32_t state_class;

    previous = start;
    current = start->next;
    subsequent = start->next->next;

    strcpy(str, TRACEFOLDER);
    strcat(str, TRACEFILE);
    sprintf(filepath, str, t);
    
    if (t==1)
    {
        fp = fopen(filepath, "w+");
    }

    if(t==1 || PRINTALLNONPROFILEDTRACES){
        strcpy(str, NONPROFILEDFOLDER);
        strcat(str, NONPROFILEDFILE);
        sprintf(filepath, str, t);
        fp_nonprofiled = fopen(filepath, "w+");
    }
    int inst_index = 0;
    double LDR_state_data = 0;
    double STR_state_data = 0;
    double EORS_state_data = 0;

    while(subsequent->next != NULL){
        differentialvoltage = 0;
        PrvInstr_data = 0; SubInstr_data = 0; Operand1_data = 0; Operand2_data = 0; BitFlip1_data = 0; BitFlip2_data = 0; HWOp1PrvInstr_data = 0; HWOp2PrvInstr_data = 0; HDOp1PrvInstr_data = 0; HDOp2PrvInstr_data = 0; HWOp1SubInstr_data = 0; HWOp2SubInstr_data = 0; HDOp1SubInstr_data = 0; HDOp2SubInstr_data = 0; Operand1_bitinteractions_data = 0; Operand2_bitinteractions_data = 0; BitFlip1_bitinteractions_data = 0; BitFlip2_bitinteractions_data = 0;
        
        Op1Op2_Overwrite_data = 0;
        NM_Op1Op2_Overwrite_data = 0;
        NM_Op1_Overwrite_data = 0;
        NM_Op2_Overwrite_data = 0;
        State_data = 0;
        LDR_state_data = 0;
        STR_state_data = 0;
        PowerModel* pwModel = &pwNoState;
        
        instructiontype = current->instruction_typedec;
        // Test for key guessing space
        // if(t == 1)
        //      keyflowfailtest(current);
        //
        uint32_t current_state = 0;
        if (current->state_class == 10 || current->state_class == 11) /* PUSH POP*/
        {
            current_state = current->result;
        }
        else
        {
            current_state = current->op2;
        }
 
        // Modelled differential voltage is total of different factors (for debugging)
        if (0)
        {
            fprintf(fdumpfile, "%x %x %x\n", current->pc, current->op1, current->op2);

           /* for (i=0;i<STATE_CLASS_COUNT;i++)
            {
                double v = pwModel->HDState[i][instructiontype]*hdistance(states[i], current_state);
                State_data += v;

                fprintf(fdumpfile, "%x %lf %d\n", current->pc, pwModel->HDState[i][instructiontype], hdistance(states[i], current_state) );
            }*/

        }

       
        if(instructiontype == 5){

            current->instruction_type[0] = 1;
            instructiontype = 0;

            if(t==1 || PRINTALLNONPROFILEDTRACES)
                fprintf(fp_nonprofiled,"%d %x\n",index,current->pc);
        }

        else{

            for (i=0;i<STATE_CLASS_COUNT;i++)
            {
                int bit;
                double v=0;
                uint32_t statebitflip = states[i] ^ current_state;
                for (bit=0;bit<32;bit++)
                {
                    v += pwModel->HDState[i*STATE_CLASS_COUNT + bit][instructiontype] * GET_BIT(bit, statebitflip);
                }
                //double v = pwModel->HDState[i][instructiontype]*hdistance(states[i], current_state);
                state_cum_data[i] = v;
                
                State_data += v; 
            }

            Op1Op2_Overwrite_data = pwModel->HDOverwrite[0][instructiontype]*hdistance(current->op1 ,current->op2);
            NM_Op1Op2_Overwrite_data = hdistance(current->op1 ,current->op2);
            NM_Op1_Overwrite_data = hdistance(current->op1 ,previous->op1);
            NM_Op2_Overwrite_data = hdistance(current->op2 ,previous->op2);

 
            hw_op1 = hweight(current->op1);
            hw_op2 = hweight(current->op2);

            hd_op1 = hdistance(previous->op1, current->op1);
            hd_op2 = hdistance(previous->op2, current->op2);



            for(i=0;i<32;i++){

                if(current->op1_binary[i] == previous->op1_binary[i])
                    current->op1_bitflip[i] = 0;
                else
                    current->op1_bitflip[i] = 1;

                if(current->op2_binary[i] == previous->op2_binary[i])
                    current->op2_bitflip[i] = 0;
                else
                    current->op2_bitflip[i] = 1;
            }

            // For each bit of two inputs

            for(i=0;i<32;i++){

                // Input hamming weights
                Operand1_data = Operand1_data + pwModel->Operand1[i][instructiontype]*current->op1_binary[i];
                Operand2_data = Operand2_data + pwModel->Operand2[i][instructiontype]*current->op2_binary[i];

                // Input hamming distance
                BitFlip1_data = BitFlip1_data + pwModel->BitFlip1[i][instructiontype]*current->op1_bitflip[i];
                BitFlip2_data = BitFlip2_data + pwModel->BitFlip2[i][instructiontype]*current->op2_bitflip[i];

            }
            // For each instruction type

            for(i=0;i<4;i++){

                // Previous and subsequent factors
                PrvInstr_data = PrvInstr_data + pwModel->PrvInstr[i][instructiontype]*previous->instruction_type[i+1];
                SubInstr_data = SubInstr_data + pwModel->SubInstr[i][instructiontype]*subsequent->instruction_type[i+1];

                // Hamming weight of previous
                HWOp1PrvInstr_data = HWOp1PrvInstr_data + pwModel->HWOp1PrvInstr[i][instructiontype]*previous->instruction_type[i+1]*hw_op1;
                HWOp2PrvInstr_data = HWOp2PrvInstr_data + pwModel->HWOp2PrvInstr[i][instructiontype]*previous->instruction_type[i+1]*hw_op2;

                // Hamming distance of previous
                HDOp1PrvInstr_data = HDOp1PrvInstr_data + pwModel->HDOp1PrvInstr[i][instructiontype]*previous->instruction_type[i+1]*hd_op1;
                HDOp2PrvInstr_data = HDOp2PrvInstr_data + pwModel->HDOp2PrvInstr[i][instructiontype]*previous->instruction_type[i+1]*hd_op2;

                // Hamming weight of subsequence
                HWOp1SubInstr_data = HWOp1SubInstr_data + pwModel->HWOp1SubInstr[i][instructiontype]*subsequent->instruction_type[i+1]*hw_op1;
                HWOp2SubInstr_data = HWOp2SubInstr_data + pwModel->HWOp2SubInstr[i][instructiontype]*subsequent->instruction_type[i+1]*hw_op2;
 
                // Hamming distance of subsequent
                HDOp1SubInstr_data = HDOp1SubInstr_data + pwModel->HDOp1SubInstr[i][instructiontype]*subsequent->instruction_type[i+1]*hd_op1;
                HDOp2SubInstr_data = HDOp2SubInstr_data + pwModel->HDOp2SubInstr[i][instructiontype]*subsequent->instruction_type[i+1]*hd_op2;

            }
            // Higher order bit interactions

            count = 0;
            
            for(i=0;i<32;i++){
                for(j=i+1;j<32;j++){
                    
                    // Input hamming weights
                    Operand1_bitinteractions_data = Operand1_bitinteractions_data + 
                        pwModel->Operand1_bitinteractions[count][instructiontype]*current->op1_binary[i]*current->op1_binary[j];
                    Operand2_bitinteractions_data = Operand2_bitinteractions_data + 
                        pwModel->Operand2_bitinteractions[count][instructiontype]*current->op2_binary[i]*current->op2_binary[j];

                    // Input hamming distance
                    BitFlip1_bitinteractions_data = BitFlip1_bitinteractions_data + 
                        pwModel->BitFlip1_bitinteractions[count][instructiontype]*current->op1_bitflip[i]*current->op1_bitflip[j];
                    BitFlip2_bitinteractions_data = BitFlip2_bitinteractions_data + 
                        pwModel->BitFlip2_bitinteractions[count][instructiontype]*current->op2_bitflip[i]*current->op2_bitflip[j];

                    count++;

                }
            }
            
            count = 0;
    
            differentialvoltage = pwModel->constant[instructiontype] + PrvInstr_data + SubInstr_data + Operand1_data + Operand2_data +
                BitFlip1_data + BitFlip2_data + HWOp1PrvInstr_data + HWOp2PrvInstr_data + HDOp1PrvInstr_data + HDOp2PrvInstr_data +
                HWOp1SubInstr_data + HWOp2SubInstr_data + HDOp1SubInstr_data + HDOp2SubInstr_data + Operand1_bitinteractions_data +
                Operand2_bitinteractions_data + BitFlip1_bitinteractions_data + BitFlip2_bitinteractions_data +
                State_data + Op1Op2_Overwrite_data;
            //EORS_state_data = State_data; 
            //fprintf(fdumpfile, "%lf\n", differentialvoltage);
        }

               
        if (current->state_class == 10 || current->state_class == 11)
        {
            // as leakage matrix(2) shows that push/pop can stop any leakage...
            states[ST_CLS_STR] = current->result;
            states[ST_CLS_EORS] = current->result;
            states[ST_CLS_LDR] = current->result;
            states[ST_CLS_MOVS] = current->result;
            
            states_op1_reg[ST_CLS_STR] = ST_REG_NONE;
            states_op1_reg[ST_CLS_EORS] = ST_REG_NONE;
            states_op1_reg[ST_CLS_LDR] = ST_REG_NONE;
            states_op1_reg[ST_CLS_MOVS] = ST_REG_NONE;

            states_op2_reg[ST_CLS_STR] = ST_REG_NONE;
            states_op2_reg[ST_CLS_EORS] = ST_REG_NONE;
            states_op2_reg[ST_CLS_LDR] = ST_REG_NONE;
            states_op2_reg[ST_CLS_MOVS] = ST_REG_NONE;

        }
        else
        {
            states[current->state_class] = current->op2;
            // when str instruction is run the register at op2
            // is set as the connection to the memory bus and therefore
            // leakage is present when the value at that register is 
            // changed due to a leaky value from a former instruction
            // it is effectively resetting str state through another
            // instruction
            // * reg_op1 is used because it is the dest register
            
            dataflow* newinst=previous;
            if (current->state_class != ST_CLS_STR && newinst->state_class != ST_CLS_STR && /* if this intr is str it has priority */
                    states_op2_reg[ST_CLS_STR] == newinst->reg_op1 && states_op2_reg[ST_CLS_STR] != ST_REG_NONE /* registers must match */)  
            {
                states[ST_CLS_STR] = newinst->result;
            }
            // similar effects also happen for ALU instructions
            if (current->state_class != ST_CLS_EORS && newinst->state_class != ST_CLS_EORS && 
                    states_op2_reg[ST_CLS_EORS] == newinst->reg_op1 && states_op2_reg[ST_CLS_EORS] != ST_REG_NONE)
            {
                states[ST_CLS_EORS] = newinst->result;
            }

            states_op1_reg[current->state_class] = current->reg_op1;
            states_op2_reg[current->state_class] = current->reg_op2;
        
        }

        // Convert from differential voltage to power
        calc_props(pwModel->constant[instructiontype], 0, inst_index); //A
        calc_props(PrvInstr_data, 1, inst_index); //B
        calc_props(SubInstr_data, 2, inst_index); //C
        calc_props(Operand1_data, 3, inst_index); //D
        calc_props(Operand2_data, 4, inst_index); //E
        calc_props(BitFlip1_data, 5, inst_index); //F
        calc_props(BitFlip2_data, 6, inst_index); //G
        calc_props(HWOp1PrvInstr_data, 7, inst_index); //H
        calc_props(HWOp2PrvInstr_data, 8, inst_index); //I
        calc_props(HDOp1PrvInstr_data, 9, inst_index); //J
        calc_props(HDOp2PrvInstr_data, 10, inst_index); //k
        calc_props(HWOp1SubInstr_data, 11, inst_index); //l
        calc_props(HWOp2SubInstr_data, 12, inst_index); //m
        calc_props(HDOp1SubInstr_data, 13, inst_index); //n
        calc_props(HDOp2SubInstr_data, 14, inst_index); //o
        calc_props(Operand1_bitinteractions_data, 15, inst_index); //p
        calc_props(Operand2_bitinteractions_data, 16, inst_index); //q
        calc_props(BitFlip1_bitinteractions_data, 17, inst_index); //r
        calc_props(BitFlip2_bitinteractions_data, 18, inst_index); //s
        calc_props(state_cum_data[ST_CLS_LDR], 19, inst_index); //t
        calc_props(state_cum_data[ST_CLS_STR], 20, inst_index); //u
        calc_props(NM_Op1Op2_Overwrite_data, 21, inst_index); //v
        calc_props(NM_Op1_Overwrite_data, 22, inst_index); //w
        calc_props(NM_Op2_Overwrite_data, 23, inst_index); //x
        calc_props(state_cum_data[ST_CLS_EORS], 24, inst_index); //y
        calc_props(differentialvoltage, 25, inst_index); //z
        set_inst_type(instructiontype, inst_index);
        inst_index ++;
#ifdef POWERTRACES
        supplycurrent = differentialvoltage/RESISTANCE;
        power = supplycurrent*SUPPLYVOLTAGE;
#else
        power = differentialvoltage;
#endif

        if(instructiontype == 2 | instructiontype == 3){
            if(CYCLEACCURATE){
                if (t == 1)
                {
#ifdef BINARYTRACES
                    fwrite(&power, sizeof(power), 1, fp);
                    fwrite(&power, sizeof(power), 1, fp);
#else
                    fprintf(fp,"%0.40f\n",power);
                    fprintf(fp,"%0.40f\n",power);
#endif
                }
                index += 2;
            }
            else{
                if (t == 1)
                {
#ifdef BINARYTRACES
                    fwrite(&power, sizeof(power), 1, fp);
#else
                    fprintf(fp,"%0.40f\n",power);
#endif
                }
                index += 1;
            }
        }
        else{
            if (t == 1)
            {
#ifdef BINARYTRACES
                fwrite(&power, sizeof(power), 1, fp);
#else
                fprintf(fp,"%0.40f\n",power);
#endif
            }
            index += 1;
        }

        previous = previous->next;
        current = current->next;
        subsequent = subsequent->next;

    }
//    calc_props_fin();

    if (t == 1)
    {
        fclose(fp);
    }

    if(t==1 || PRINTALLNONPROFILEDTRACES) fclose(fp_nonprofiled);

}

#endif

//-------------------------------------------------------------------

void freedataflow(){

    previous = start;
    subsequent = previous->next;

    while((previous = subsequent) != NULL){

        subsequent = subsequent->next;
        free (previous);

    }

}
