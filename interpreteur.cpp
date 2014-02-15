#include "interpreteur.h"


Interpreteur::Interpreteur(MainWindow &getMainWindow)
{
	mainWindow = &getMainWindow;
	R_PC = 0x300200;
	NextInstruction="NOP";
	DelaySlot = NULL;
	mainWindow->connect(mainWindow, SIGNAL(nextInstruct()), this, SLOT(slotNext()));
	mainWindow->connect(mainWindow, SIGNAL(registerChanged(QTableWidgetItem *,QTableWidget*)), this, SLOT(registerChange(QTableWidgetItem *, QTableWidget*)));
	//Init des regsitres
		for(short i = 0; i<15 ; i++)
			R[i]=0;
		R[4]=0x1;
		mainWindow->setRegister(tr("R4"), QString::number(R[4],16));
		R[15]=0x88023f98;
		mainWindow->setRegister(tr("R15"), QString::number(R[15],16));
		R_MACH=0;
		R_MACL=0;
		R_GBR=0;
		R_PR=0xA0056694;
		mainWindow->setRegister(tr("PR"), QString::number(0xA0056694,16));
}

void Interpreteur::slotNext()
{
	nextStep();
	//qDebug() << memRead(0x300209, 'B');
	//memRead(0x300209, 'B');
}

void Interpreteur::registerChange(QTableWidgetItem *item, QTableWidget *registerList)
{
	unsigned long value = item->text().toULong(0,16);
	QString registre = registerList->item(item->row(),0)->text();
	if(registre[0] == 'R')
	{
		short registreNumber;
		if(registre[2].isDigit())
			registreNumber = QString(registre[1]).toShort()*10 + QString(registre[2]).toShort();
		else
			registreNumber = QString(registre[1]).toShort();
		R[registreNumber] = value;
	}
	else if(registre == "PC")
		R_PC = value;
	else if(registre == "MACH")
		R_MACH = value;
	else if(registre == "MACL")
		R_MACL = value;
	else if(registre == "PR")
		R_PR = value;
}

void Interpreteur::memWrite(unsigned long adresse,unsigned long value, QChar type)
{

	if(type == 'B')
	{
		if(adresse%2 == 1)//est impaire
		{
			value = (memRead(adresse-1, 'B')<<8) + (value&0xFF);
			mainWindow->addInstruction(QString::number(adresse-1,16),QString::number(value,16).rightJustified(4,'0'),"");
		}
		else
		{
			value =  ((value&0xFF)<<8) + memRead(adresse, 'B');
			mainWindow->addInstruction(QString::number(adresse,16),QString::number(value,16).rightJustified(4,'0'),"");
		}
	}
	if(type == 'W')
	{
		if(adresse%2 == 1)//est impaire -- Apparement produit une erreur sur le vrai emulateur
		{
			//Byte 1
				memWrite(adresse,(value>>8), 'B');
			//Byte 2
				memWrite(adresse+1,(value&0xFF), 'B');
		}
		else
		{
			value =  value&0xFFFF;
			mainWindow->addInstruction(QString::number(adresse,16),QString::number(value,16).rightJustified(4,'0'),"");
		}
	}
	if(type== 'L')
	{
		if(adresse%2 == 1)//est impaire -- Apparement produit une erreur sur le vrai emulateur
		{
			//Byte 1
				memWrite(adresse,(value>>24), 'B');
			//Byte 2 et 3
				memWrite(adresse+1,((value>>8)&0xFFFF), 'W');
			//Byte 4
				memWrite(adresse+3,(value&0xFF), 'B');
		}
		else
		{
			//Byte 1 et 2
				memWrite(adresse,((value>>16)&0xFFFF), 'W');
			//Byte 3 et 4
				memWrite(adresse+2,((value)&0xFFFF), 'W');
		}
	}
}

unsigned long Interpreteur::memRead(unsigned long adresse, QChar type)
{
	unsigned long out = 0;
	if(type == 'B')
	{
		if(adresse%2 == 1)//est impaire
			out = mainWindow->readInstructionContent(QString::number(adresse-1,16))&0xFF;
		else
			out = mainWindow->readInstructionContent(QString::number(adresse,16)) >> 8;
	}
	else if(type == 'W' || type == 'L')
	{
		out = (memRead(adresse,'B')<<8) + memRead(adresse+1,'B');
		if(type == 'L')
		{
			out<<=16;
			out+= (memRead(adresse+2,'B')<<8) + memRead(adresse+3,'B');
		}
	}
	return out;
}

void Interpreteur::Delay_Slot(unsigned long adresse)
{
	DelaySlot = adresse;
}

void Interpreteur::nextStep(bool show)
{
	QString instruction;
	if(DelaySlot != NULL)
	{
		instruction = mainWindow->readInstructionInterpretation(QString::number(DelaySlot,16));
		DelaySlot = NULL;
		R_PC -= 2;
	}
	else
		instruction = mainWindow->readInstructionInterpretation(QString::number(R_PC,16));



		//instruction.right(instruction.indexOf(' '));
		QString opName = instruction;
		opName.truncate(instruction.indexOf(' '));
		bool detected = false;
		unsigned short m;
		unsigned short n;
		unsigned short immdisp;
		//qDebug() << opName<< endl;
		//qDebug() << instruction;
	//Execution de l'instruction
		//MOV
		if(opName.toLower() == "mov" || opName.toLower().mid(0,4)== "mov.")
		{
			//MOV Rm,Rn - ok
				QRegularExpressionMatch match;
				QRegularExpression instructRegEx ("^[\t ]*MOV[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
				instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				if(instruction.contains(instructRegEx, &match))
				{
					detected=true;
					m = match.capturedTexts().value(1).toUShort();
					n = match.capturedTexts().value(2).toUShort();
					//Execution
					R[n]=R[m];
					mainWindow->setRegister("R" + match.capturedTexts().value(2),QString::number(R[n],16));
					R_PC+=2;
				}
			//MOV.* Rm,@Rn - ok
				if(!detected)
				{
					QRegularExpressionMatch match;
					QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*@R([0-9]{1,2})[\t ]*$");
					instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
					if(instruction.contains(instructRegEx, &match))
					{
						detected=true;
						m = match.capturedTexts().value(2).toUShort();
						n = match.capturedTexts().value(3).toUShort();
						//Execution
						memWrite(R[n],R[m],match.capturedTexts().value(1)[0]);
						R_PC+=2;
					}
				}
			//MOV.* @Rm,Rn - ok
				if(!detected)
				{
					QRegularExpressionMatch match;
					QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+@R([0-9]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
					instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
					if(instruction.contains(instructRegEx, &match))
					{
						detected=true;
						m = match.capturedTexts().value(2).toUShort();
						n = match.capturedTexts().value(3).toUShort();
						//Execution
						R[n]=(long)memRead(R[m],match.capturedTexts().value(1)[0]);
						if(match.capturedTexts().value(1)[0] == 'B')
						{
							if ((R[n]&0x80)==0)  R[n]&=0x000000FF;
							else R[n]|=0xFFFFFF00;
						}
						else if(match.capturedTexts().value(1)[0] == 'W')
						{
							if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
							else R[n]|=0xFFFF0000;
						}
						R_PC+=2;
						mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
					}
				}
			//MOV.* Rm,@–Rn - ok // le - permet d'indiquer qu'ici, l'adresse indique la fin du dernier octet (contrairement a d'habitude où l'adresse indique le début du premier octet)
				if(!detected)
				{
					QRegularExpressionMatch match;
					QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*@-R([0-9]{1,2})[\t ]*$");
					instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
					if(instruction.contains(instructRegEx, &match))
					{
						detected=true;
						m = match.capturedTexts().value(2).toUShort();
						n = match.capturedTexts().value(3).toUShort();
						//Execution
						if(match.capturedTexts().value(1)[0] == 'B')
							R[n]=R[n]-1;
						else if(match.capturedTexts().value(1)[0] == 'W')
							R[n]=R[n]-2;
						else//L
							R[n]=R[n]-4;
						memWrite(R[n],R[m],match.capturedTexts().value(1)[0]);
						R_PC+=2;
						mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
					}
				}
				//MOV.* @Rm+,Rn - ok
				if(!detected)
				{
					QRegularExpressionMatch match;
					QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+@R([0-9]{1,2})\\+[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
					instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
					if(instruction.contains(instructRegEx, &match))
					{
						detected=true;
						m = match.capturedTexts().value(2).toUShort();
						n = match.capturedTexts().value(3).toUShort();
						//Execution
						R[n]=(long)memRead(R[m],match.capturedTexts().value(1)[0]);
						if(match.capturedTexts().value(1)[0] == 'B')
						{
							if ((R[n]&0x80)==0) R[n]&=0x000000FF;
							else R[n]|=0xFFFFFF00;
							if (n!=m) R[m]+=1;
						}
						else if(match.capturedTexts().value(1)[0] == 'W')
						{
							if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
							else R[n]|=0xFFFF0000;
							if (n!=m) R[m]+=2;
						}
						else//L
						{
							if (n!=m) R[m]+=4;
						}
						R_PC+=2;
						mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
						mainWindow->setRegister("R" + match.capturedTexts().value(2),QString::number(R[m],16));
					}
				}
				//MOV.* Rm,@(R0,Rn) - ok
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*@\\([\t ]*R0[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							m = match.capturedTexts().value(2).toUShort();
							n = match.capturedTexts().value(3).toUShort();
							//Execution
							memWrite(R[n]+R[0],R[m],match.capturedTexts().value(1)[0]);
							R_PC+=2;
						}
					}
				//MOV.* @(R0,Rm),Rn
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+@\\([\t ]*R0[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							m = match.capturedTexts().value(2).toUShort();
							n = match.capturedTexts().value(3).toUShort();
							//Execution
							R[n]=(long)memRead(R[m]+R[0],match.capturedTexts().value(1)[0]);

							if(match.capturedTexts().value(1)[0] == 'B')
							{
								if ((R[n]&0x80)==0) R[n]&0x000000FF;
								else R[n]|=0xFFFFFF00;
							}
							else if(match.capturedTexts().value(1)[0] == 'W')
							{
								if ((R[n]&0x8000)==0) R[n]&0x0000FFFF;
								else R[n]|=0xFFFF0000;
							}
							R_PC+=2;
							mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
						}
					}
				//MOV #imm,Rn - ok
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV[\t ]+#H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							n = match.capturedTexts().value(2).toUShort();
							immdisp = match.capturedTexts().value(1).toUShort(0,16);
							//Execution
							if ((immdisp&0x80)==0) R[n]=(0x000000FF & (long)immdisp);
							else R[n]=(0xFFFFFF00 | (long)immdisp);
							R_PC+=2;
							mainWindow->setRegister("R" + match.capturedTexts().value(2),QString::number(R[n],16));
						}
					}
				//MOV.* @(disp,PC),Rn - ok
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx("^[\t ]*MOV\\.([WL])[\t ]+@\\([\t ]*H'([0-9A-Fa-f]{1,3})[\t ]*,[\t ]*PC[\t ]*\\)[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							n = match.capturedTexts().value(3).toUShort();
							if((match.capturedTexts().value(1)[0] == 'W' && immdisp%2!=0) || (match.capturedTexts().value(1)[0] == 'L' && immdisp%4!=0))
							{
								detected=false;
								qDebug() << "MOV.* @(disp,PC),Rn : la valeur de disp doit être un multiple de son nombre d'octet (2 pour W et 4 pour L)";
							}
							else
							{
								//Execution
								if(match.capturedTexts().value(1)[0] ==  'W')
								{
									R[n]=memRead(R_PC+4+((immdisp)), 'W');
									if ((R[n]&0x8000)==0) R[n]&=0x0000FFFF;
									else R[n]|=0xFFFF0000;
									qDebug() <<R_PC+4+((immdisp));
								}
								else
								{
									R[n]=memRead(((R_PC&0xFFFFFFFC)+4)+((immdisp)),'L');
									qDebug() <<((R_PC&0xFFFFFFFC)+4)+((immdisp));
								}
								mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
								R_PC+=2;
							}
						}
					}
				//MOV.* @(disp,GBR),R0
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*GBR[\t ]*\\)[\t ]*,[\t ]*R0[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							long disp;
							//Execution
							disp=(0x000000FF & (long)immdisp);
							R[0]=(long)memRead(R_GBR+disp,match.capturedTexts().value(1)[0]);

							if(match.capturedTexts().value(1)[0] ==  'B')
							{
								if ((R[0]&0x80)==0) R[0]&=0x000000FF;
								else R[0]|=0xFFFFFF00;
							}
							else if(match.capturedTexts().value(1)[0] ==  'W')
							{
								if ((R[0]&0x8000)==0) R[0]&=0x0000FFFF;
								else R[0]|=0xFFFF0000;
							}
							mainWindow->setRegister("R0",QString::number(R[0],16));
							R_PC+=2;
						}
					}
				//MOV.* R0,@(disp,GBR)
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BWL])[\t ]+R0[\t ]*,[\t ]*@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*GBR[\t ]*\\)[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							//Execution
							long disp;
							disp=(0x000000FF & (long)immdisp);
							memWrite(R_GBR+disp,R[0],match.capturedTexts().value(1)[0]);
							R_PC+=2;
						}
					}
				//MOV.[BW] R0,@(disp,Rn)
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BW])[\t ]+R0[\t ]*,[\t ]*@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							n = match.capturedTexts().value(3).toUShort();
							//Execution
							long disp;
							disp=(0x0000000F & (long)immdisp);
							memWrite(R[n]+disp,R[0],match.capturedTexts().value(1)[0]);
							R_PC+=2;
						}
					}
				//MOV.L Rm,@(disp,Rn)
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV.L[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							m = match.capturedTexts().value(1).toUShort();
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							n = match.capturedTexts().value(3).toUShort();
							long disp;
							//Execution
							disp=(0x0000000F & (long)immdisp);
							memWrite(R[n]+(disp),R[m],'L');
							R_PC+=2;
						}
					}
				//MOV.[BW] @(disp,Rm),R0
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV\\.([BW])[\t ]+@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*,[\t ]*R0[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							m = match.capturedTexts().value(3).toUShort();
							long disp;
							//Execution
							disp=(0x0000000F & (long)immdisp);
							R[0]=memRead(R[m]+disp+2,match.capturedTexts().value(1)[0]);
							if(match.capturedTexts().value(1)[0] == 'B')
							{
								if ((R[0]&0x80)==0) R[0]&=0x000000FF;
								else R[0]|=0xFFFFFF00;
							}
							else//W
							{
								if ((R[0]&0x8000)==0) R[0]&=0x0000FFFF;
								else R[0]|=0xFFFF0000;
							}
							mainWindow->setRegister("R0",QString::number(R[0],16));
							R_PC+=2;
						}
					}
				//MOV.L @(disp,Rm),Rn
					if(!detected)
					{
						QRegularExpressionMatch match;
						QRegularExpression instructRegEx ("^[\t ]*MOV.L[\t ]+[\t ]*@\\([\t ]*H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*\\)[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
						instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
						if(instruction.contains(instructRegEx, &match))
						{
							detected=true;
							m = match.capturedTexts().value(1).toUShort();
							immdisp = match.capturedTexts().value(2).toUShort(0,16);
							n = match.capturedTexts().value(3).toUShort();
							//Execution
							long disp;
							disp=(0x0000000F & (long)immdisp);
							R[n]=memRead(R[m]+(disp),'L');
							R_PC+=2;
							mainWindow->setRegister("R" + match.capturedTexts().value(3),QString::number(R[n],16));
						}
					}
		}
		else if(opName.toLower() == "jmp") //JMP @Rn
		{
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*JMP[\t ]+@R([0-9]{1,2})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				m = match.capturedTexts().value(1).toUShort();
				//Execution
				unsigned long temp;
				temp=R_PC;
				R_PC=R[m];
				Delay_Slot(temp+2);
			}
		}
		else if(opName.toLower() == "sts") //STS (MACH|MACL|PR),Rn
		{
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*STS[\t ]+(MACH|MACL|PR)[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				n = match.capturedTexts().value(2).toUShort();
				//Execution
				if(match.capturedTexts().value(1) == "MACH")
				{
					R[n]=R_MACH;
					if((R[n]&0x00000200)==0)
						R[n]&=0x000003FF;
					else
						R[n]|=0xFFFFFC00;
					R_PC+=2;
				}
				else if(match.capturedTexts().value(1) == "MACL")
				{
					R[n]=R_MACL;
					R_PC+=2;
				}
				else if(match.capturedTexts().value(1) == "PR")
				{
					R[n]=R_PR;
					R_PC+=2;
				}
			}
		}
		else if(opName.toLower() == "sts.l")
		{
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*STS\.L[\t ]+(MACH|MACL|PR)[\t ]*,[\t ]*@-R([0-9]{1,2})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				n = match.capturedTexts().value(2).toUShort();
				//Execution
				if(match.capturedTexts().value(1) == "MACH")
				{
					R[n] = R[n] - 4;
					if((R_MACH&0x00000200)==0)
						memWrite(R[n],R_MACH&0x000003FF, 'L');
					else
						memWrite(R[n],R_MACH|0xFFFFFC00, 'L');
				}
				else if(match.capturedTexts().value(1) == "MACL")
				{
					R[n] = R[n] - 4;
					memWrite(R[n],R_MACL, 'L');
				}
				else if(match.capturedTexts().value(1) == "PR")
				{
					R[n] = R[n] - 4;
					memWrite(R[n],R_PR, 'L');
				}
				R_PC+=2;
				mainWindow->setRegister("R" + match.capturedTexts().value(2),QString::number(R[n],16));
			}
		}
		else if(opName.toLower() == "add")
		{
			//ADD Rm,Rn
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*ADD[\t ]+R([0-9]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				n = match.capturedTexts().value(1).toUShort();
				m = match.capturedTexts().value(2).toUShort();
				//Execution
				R[n]+=R[m];
				R_PC+=2;
				mainWindow->setRegister("R" + match.capturedTexts().value(1),QString::number(R[n],16));

			}
			//ADD #imm,Rn
			if(!detected)
			{
				QRegularExpressionMatch match;
				QRegularExpression instructRegEx ("^[\t ]*ADD[\t ]+#H'([0-9A-Fa-f]{1,2})[\t ]*,[\t ]*R([0-9]{1,2})[\t ]*$");
				instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
				if(instruction.contains(instructRegEx, &match))
				{
					detected=true;
					immdisp = match.capturedTexts().value(1).toUShort(0,16);
					n = match.capturedTexts().value(2).toUShort();
					//Execution
					if ((immdisp&0x80)==0)
						R[n]+=(0x000000FF & (long)immdisp);
					else
						R[n]+=(0xFFFFFF00 | (long)immdisp);
					R_PC+=2;
					mainWindow->setRegister("R" + match.capturedTexts().value(2),QString::number(R[n],16));
				}
			}
		}
		else if(opName.toLower() == "bsr") //BSR disp
		{
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*BSR[\t ]+H'([0-9A-Fa-f]{1,6})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				immdisp = match.capturedTexts().value(1).toUShort(0,16);
				//Execution
				long disp;
				if ((immdisp&0x800)==0) disp=(0x00000FFF & immdisp);
				else disp=(0xFFFFF000 | immdisp);
				R_PR=R_PC+4;
				R_PC=R_PC+(disp)+4;
				Delay_Slot(R_PR-2);
				mainWindow->setRegister("PR",QString::number(R_PR,16));
			}
		}
		else if(opName.toLower() == "jsr") //JSR @Rm
		{
			QRegularExpressionMatch match;
			QRegularExpression instructRegEx ("^[\t ]*JSR[\t ]+@R([0-9]{1,2})[\t ]*$");
			instructRegEx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			if(instruction.contains(instructRegEx, &match))
			{
				detected=true;
				m = match.capturedTexts().value(1).toUShort();
				//Execution
				R_PR=R_PC+4;
				R_PC=R[m];
				Delay_Slot(R_PR-2);
				mainWindow->setRegister("PR",QString::number(R_PR,16));
			}
		}
	//Sinon Passage de la ligne
		if(!detected)
			R_PC+=2;

	//Activation de la ligne suivante
		if(DelaySlot != NULL)
			mainWindow->setActiveLine(QString::number(DelaySlot,16));
		else
			mainWindow->setActiveLine(QString::number(R_PC,16));
	//Mise à jour du registre PC
		mainWindow->setRegister("PC",QString::number(R_PC,16));

}

void Interpreteur::loadFile(char* filename)
{
	QFile fichier(filename);
	//std::ifstream fichier(filename, std::ios::in);
	if(fichier.open(QIODevice::ReadOnly))
	{
		fichier.seek(8);
		   char c;
		   unsigned char u_c;
		   fichier.getChar(&c);
		   u_c = c;
		   if(true || u_c == 0x0c)
		   {
			   //Debut du code à l'offset 0x200
			   fichier.seek(0x200);
			   char buffer[2];
			   unsigned int ligne=0;
			   while(fichier.read(buffer,2))
			   {
				   unsigned char nibble[4];
				   nibble[0] = ((unsigned)buffer[0]); nibble[0] >>= 4;
				   nibble[1] = ((unsigned)buffer[0]); nibble[1] &= 0xF;
				   nibble[2] = ((unsigned)buffer[1]); nibble[2] >>= 4;
				   nibble[3] = ((unsigned)buffer[1]); nibble[3] &= 0xF;

			//Interpretation
				QString interpretation="";
				switch(nibble[0])
				{
					case b0000:
						switch(nibble[3])
						{
							case b1000:
								switch(nibble[2]) {
									case b0000: interpretation = "CLRT";	break;
									case b0001: interpretation = "SETT";	break;
									case b0010: interpretation = "CLRMAC";	break;
									case b0011: interpretation = "LDTLB";	break;
									case b0100: interpretation = "CLRS";	break;
									case b0101: interpretation = "SETS";	break;
								} break;
							case b1001:
								switch(nibble[2]) {
									case b0000: interpretation = "NOP";	break;
									case b0001: interpretation = "DIV0U";	break;
									case b0010: interpretation = "MOVT R" +  QString::number(nibble[1]);	break;
								} break;
							case b1011:
								switch(nibble[2]) {
									case b0000: interpretation = "RTS";	break;
									case b0001: interpretation = "SLEEP";	break;
									case b0010: interpretation = "RTE";	break;
								} break;
							case b0010:
								switch(nibble[2]){
									case b0000: interpretation = "STC SR, R" + QString::number(nibble[1]);	break; //0000nnnn00000010
									case b0001: interpretation = "STC GBR, R" + QString::number(nibble[1]);	break; //0000nnnn00010010
									case b0010: interpretation = "STC VBR, R" + QString::number(nibble[1]);	break; //0000nnnn00100010
									case b0011: interpretation = "STC SSR, R" + QString::number(nibble[1]);	break; //0000nnnn00110010
									case b0100: interpretation = "STC SPC, R" + QString::number(nibble[1]);	break; //0000nnnn01000010
									//case b0101: interpretation = "STC MOD, R" + QString::number(nibble[1]);	break; //0000nnnn01010010 - SH3-DSP only
									//case b0110: interpretation = "STC RS, R" + QString::number(nibble[1]);	break; //0000nnnn01100010 - SH3-DSP only
									//case b0111: interpretation = "STC RE, R" + QString::number(nibble[1]);	break; //0000nnnn01110010 - SH3-DSP only
									case b1000: interpretation = "STC R0_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn10000010
									case b1001: interpretation = "STC R1_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn10010010
									case b1010: interpretation = "STC R2_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn10100010
									case b1011: interpretation = "STC R3_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn10110010
									case b1100: interpretation = "STC R4_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn11000010
									case b1101: interpretation = "STC R5_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn11010010
									case b1110: interpretation = "STC R6_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn11100010
									case b1111: interpretation = "STC R7_BANK, R" + QString::number(nibble[1]);	break; //0000nnnn11110010
								} break;
							case b0011:
								switch(nibble[2]){
									case b0000: interpretation = "BSRF R" + QString::number(nibble[1]);	break; //0000nnnn00000011
									case b0010: interpretation = "BRAF R" + QString::number(nibble[1]);	break; //0000nnnn00100011
									case b1000: interpretation = "PREF @R" + QString::number(nibble[1]);	break; //0000nnnn10000011
								}break;
							case b1010:
								switch(nibble[2]) {
								case b0000: interpretation = "STS MACH, R" + QString::number(nibble[1]);	break; //0000nnnn00001010
								case b0001: interpretation = "STS MACL, R" + QString::number(nibble[1]);	break; //0000nnnn00011010
								case b0010: interpretation = "STS PR, R" + QString::number(nibble[1]);	break; //0000nnnn00101010
								//case b0101: interpretation = "STS FPUL, R" + QString::number(nibble[1]);	break; //0000nnnn01011010 - SH-3E only
								//case b0110: interpretation = "STS FPSCR, R" + QString::number(nibble[1]);	break; //0000nnnn01101010 - SH-3E only
								//case b0111: interpretation = "STS A0, R" + QString::number(nibble[1]);	break; //0000nnnn01111010 - SH3-DSP only
								//case b1000: interpretation = "STS X0, R" + QString::number(nibble[1]);	break; //0000nnnn10001010 - SH3-DSP only
								//case b1001: interpretation = "STS X1, R" + QString::number(nibble[1]);	break; //0000nnnn10011010 - SH3-DSP only
								//case b1010: interpretation = "STS Y0, R" + QString::number(nibble[1]);	break; //0000nnnn10101010 - SH3-DSP only
								//case b0010: interpretation = "STS Y1, R" + QString::number(nibble[1]);	break; //0000nnnn10111010 - SH3-DSP only
								} break;
							case b0100: interpretation = "MOV.B R" + QString::number(nibble[2]) + ", @(R0,R" + QString::number(nibble[1]) + ")"; break; //0000nnnnmmmm0100
							case b0101: interpretation = "MOV.W R" + QString::number(nibble[2]) + ", @(R0,R" + QString::number(nibble[1]) + ")"; break; //0000nnnnmmmm0101
							case b0110: interpretation = "MOV.L R" + QString::number(nibble[2]) + ", @(R0,R" + QString::number(nibble[1]) + ")"; break; //0000nnnnmmmm0110
							case b1100: interpretation = "MOV.B @(R0,R" + QString::number(nibble[2]) + "), R" + QString::number(nibble[1]); break; //0000nnnnmmmm1100
							case b1101: interpretation = "MOV.W @(R0,R" + QString::number(nibble[2]) + "), R" + QString::number(nibble[1]); break; //0000nnnnmmmm1101
							case b1110: interpretation = "MOV.L @(R0,R" + QString::number(nibble[2]) + "), R" + QString::number(nibble[1]); break; //0000nnnnmmmm1110
							case b1111: interpretation = "MAC.L @R" + QString::number(nibble[2]) + "+, @R" + QString::number(nibble[1]) + "+"; break; //0000nnnnmmmm1111
						} break;
					case b0001: interpretation = "MOV.L R" + QString::number(nibble[2]) + ",@(H'" + QString::number(4*nibble[3],16) + ",R" + QString::number(nibble[1]) + ")"; break; //0001nnnnmmmmdddd
					case b0010:
						switch(nibble[3]) {
							case b0000: interpretation = "MOV.B R" + QString::number(nibble[2]) + ", @R" + QString::number(nibble[1]); break; //0010nnnnmmmm0000
							case b0001: interpretation = "MOV.W R" + QString::number(nibble[2]) + ", @R" + QString::number(nibble[1]); break; //0010nnnnmmmm0001
							case b0010: interpretation = "MOV.L R" + QString::number(nibble[2]) + ", @R" + QString::number(nibble[1]); break; //0010nnnnmmmm0010
							case b0100: interpretation = "MOV.B R" + QString::number(nibble[2]) + ", @-R" + QString::number(nibble[1]); break; //0010nnnnmmmm0100
							case b0101: interpretation = "MOV.W R" + QString::number(nibble[2]) + ", @-R" + QString::number(nibble[1]); //0010nnnnmmmm0101
							case b0110: interpretation = "MOV.L R" + QString::number(nibble[2]) + ", @-R" + QString::number(nibble[1]); //0010nnnnmmmm0110
							case b0111: interpretation = "DIV0S R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm0111
							case b1000: interpretation = "TST R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break;  //0010nnnnmmmm1000
							case b1001: interpretation = "AND R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1001
							case b1010: interpretation = "XOR R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1010
							case b1011: interpretation = "OR R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1011
							case b1100: interpretation = "CMP/STR R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1100
							case b1101: interpretation = "XTRCT R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1101
							case b1110: interpretation = "MULU.W R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1110
							case b1111: interpretation = "MULS.W R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0010nnnnmmmm1111
						} break;
					case b0011:
						switch(nibble[3]) {
							case b0000: interpretation = "CMP/EQ R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0000
							case b0010: interpretation = "CMP/HS R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0010
							case b0011: interpretation = "CMP/GE R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0011
							case b0100: interpretation = "DIV1 R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0100
							case b0101: interpretation = "DMULU.L R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0101
							case b0110: interpretation = "CMP/HI R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0110
							case b0111: interpretation = "CMP/GT R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm0111
							case b1000: interpretation = "SUB R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1000
							case b1010: interpretation = "SUBC R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1010
							case b1011: interpretation = "SUBV R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1011
							case b1100: interpretation = "ADD R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1100
							case b1101: interpretation = "DMULS.L R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1101
							case b1110: interpretation = "ADDC R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1110
							case b1111: interpretation = "ADDV R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0011nnnnmmmm1111
						} break;
					case b0100:
						switch(nibble[3]) {
							case b0110:
								switch(nibble[2]) {
									case b0000: interpretation = "LDS.L @R" + QString::number(nibble[1]) + "+,MACH";	break; //0100mmmm00000110
									case b0001: interpretation = "LDS.L @R" + QString::number(nibble[1]) + "+,MACL";	break; //0100mmmm00010110
									case b0010: interpretation = "LDS.L @R" + QString::number(nibble[1]) + "+,PR";	break; //0100mmmm00100110
									/*DSP & SH-3E :
									case b0110: interpretation = "LDS.L"; break; //0100mmmm01100110
									case b0111: interpretation = "LDS"; break; //0100mmmm01110110
									case b1000: interpretation = "LDS.L"; break; //0100mmmm10000110
									case b1001: interpretation = "LDS.L"; break; //0100mmmm10010110
									case b1010: interpretation = "LDS.L"; break; //0100mmmm10100110
									case b1011: interpretation = "LDS.L"; break; //0100mmmm10110110
									case b0101: interpretation = "LDS.L"; break; //0100nnnn01010110*/
								} break;
							case b0111:
								switch(nibble[2]){
									case b0000: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, SR"; break; //0100mmmm00000111
									case b0001: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, GBR"; break; //0100mmmm00010111
									case b0010: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, VBR"; break; //0100mmmm00100111
									case b0011: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, SSR"; break; //0100mmmm00110111
									case b0100: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, SPC"; break; //0100mmmm01000111

									case b0101: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, MOD"; break; //0100mmmm01010111
									case b0110: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, RS"; break; //0100mmmm01100111
									case b0111: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, RE"; break; //0100mmmm01110111
									case b1000: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R0_BANK"; break; //0100mmmm10000111
									case b1001: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R1_BANK"; break; //0100mmmm10010111
									case b1010: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R2_BANK"; break; //0100mmmm10100111
									case b1011: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R3_BANK"; break; //0100mmmm10110111
									case b1100: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R4_BANK"; break; //0100mmmm11000111
									case b1101: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R5_BANK"; break; //0100mmmm11010111
									case b1110: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R6_BANK"; break; //0100mmmm11100111
									case b1111: interpretation = "LDC.L @R" + QString::number(nibble[1]) + "+, R7_BANK"; break; //0100mmmm11110111
								} break;
							case b1010:
								switch(nibble[2]) {
									case b0000: interpretation = "LDS R" + QString::number(nibble[1]) + ", MACH"; break; //0100mmmm00001010
									case b0001: interpretation = "LDS R" + QString::number(nibble[1]) + ", MACL"; break; //0100mmmm00011010
									case b0010: interpretation = "LDS R" + QString::number(nibble[1]) + ", PR"; break; //0100mmmm00101010
									/* SH3-DSP et SH3E
									//case b0110: interpretation = "0100mmmm01101010      LDS";	break;
									case b0111: interpretation = "LDS"; break; //0100mmmm01111010
									case b1000: interpretation = "LDS"; break; //0100mmmm10001010
									case b1001: interpretation = "LDS"; break; //0100mmmm10011010
									case b1010: interpretation = "LDS"; break; //0100mmmm10101010
									case b1011: interpretation = "LDS"; break; //0100mmmm10111010
									case b0101: interpretation = "LDS"; break; //0100nnnn01011010*/
								} break;
							case b1110:
								switch(nibble[2]) {
									case b0000: interpretation = "LDC R" + QString::number(nibble[1]) + ", SR"; break; //0100mmmm00001110
									case b0001: interpretation = "LDC R" + QString::number(nibble[1]) + ", GBR"; break; //0100mmmm00011110
									case b0010: interpretation = "LDC R" + QString::number(nibble[1]) + ", VBR"; break; //0100mmmm00101110
									case b0011: interpretation = "LDC R" + QString::number(nibble[1]) + ", SSR"; break; //0100mmmm00111110
									case b0100: interpretation = "LDC R" + QString::number(nibble[1]) + ", SPC"; break; //0100mmmm01001110
									//case b0101: interpretation = "LDC R" + QString::number(nibble[1]) + ", MOD"; break; //0100mmmm01011110 - DSP
									//case b0110: interpretation = "LDC R" + QString::number(nibble[1]) + ", RS"; break; //0100mmmm01101110 - DSP
									//case b0111: interpretation = "LDC R" + QString::number(nibble[1]) + ", RE"; break; //0100mmmm01111110 - DSP
									case b1000: interpretation = "LDC R" + QString::number(nibble[1]) + ", R0_BANK"; break; //0100mmmm10001110
									case b1001: interpretation = "LDC R" + QString::number(nibble[1]) + ", R1_BANK"; break; //0100mmmm10011110
									case b1010: interpretation = "LDC R" + QString::number(nibble[1]) + ", R2_BANK"; break; //0100mmmm10101110
									case b1011: interpretation = "LDC R" + QString::number(nibble[1]) + ", R3_BANK"; break; //0100mmmm10111110
									case b1100: interpretation = "LDC R" + QString::number(nibble[1]) + ", R4_BANK"; break; //0100mmmm11001110
									case b1101: interpretation = "LDC R" + QString::number(nibble[1]) + ", R5_BANK"; break; //0100mmmm11011110
									case b1110: interpretation = "LDC R" + QString::number(nibble[1]) + ", R6_BANK"; break; //0100mmmm11101110
									case b1111: interpretation = "LDC R" + QString::number(nibble[1]) + ", R7_BANK"; break; //0100mmmm11111110
								} break;
							case b0100:
								switch(nibble[2]) {
									case b0001: interpretation = "SETRC R" + QString::number(nibble[1]); break; //0100mmmm00010100
									case b0000: interpretation = "ROTL R" + QString::number(nibble[1]); break; //0100nnnn00000100
									case b0010: interpretation = "ROTCL R" + QString::number(nibble[1]); break; //0100nnnn00100100
								} break;
							case b0000:
								switch(nibble[2]) {
									case b0000: interpretation = "SHLL R" + QString::number(nibble[1]); break; //0100nnnn00000000
									case b0001: interpretation = "DT R" + QString::number(nibble[1]); break; //0100nnnn00010000
									case b0010: interpretation = "SHAL R" + QString::number(nibble[1]); break; //0100nnnn00100000
								} break;
							case b0001:
								switch(nibble[2]) {
									case b0000: interpretation = "SHLR R" + QString::number(nibble[1]); break; //0100nnnn00000001
									case b0001: interpretation = "CMP/PZ R" + QString::number(nibble[1]); break; //0100nnnn00010001
									case b0010: interpretation = "SHAR R" + QString::number(nibble[1]); break; //0100nnnn00100001
								} break;
							case b0010:
								switch(nibble[2]) {
									case b0000: interpretation = "STS.L MACH, @-R" + QString::number(nibble[1]); break; //0100nnnn00000010
									case b0001: interpretation = "STS.L MACL, @-R" + QString::number(nibble[1]); break; //0100nnnn00010010
									case b0010: interpretation = "STS.L PR, @-R" + QString::number(nibble[1]); break; //0100nnnn00100010
									//case b0101: interpretation = "STS.L"; break; //0100nnnn01010010 - SH-3E
									//case b0110: interpretation = "STS.L"; break; //0100nnnn01100010 - SH-3E
									//case b0111: interpretation = "STS.L"; break; //0100nnnn01110010 - DSP
									//case b1000: interpretation = "STS.L"; break; //0100nnnn10000010 - DSP
									//case b1001: interpretation = "STS.L"; break; //0100nnnn10010010 - DSP
									//case b1010: interpretation = "STS.L"; break; //0100nnnn10100010 - DSP
									//case b1011: interpretation = "STS.L"; break; //0100nnnn10110010 - DSP
								} break;
							case b0011:
								switch(nibble[2]) {
									case b0000: interpretation = "STC.L SR, @-R" + QString::number(nibble[1]); break; //0100nnnn00000011
									case b0001: interpretation = "STC.L GBR, @-R" + QString::number(nibble[1]); break; //0100nnnn00010011
									case b0010: interpretation = "STC.L VBR, @-R" + QString::number(nibble[1]); break; //0100nnnn00100011
									case b0011: interpretation = "STC.L SSR, @-R" + QString::number(nibble[1]); break; //0100nnnn00110011
									case b0100: interpretation = "STC.L SPC, @-R" + QString::number(nibble[1]);; break; //0100nnnn01000011
									//case b0101: interpretation = "STC.L"; break; //0100nnnn01010011 - DSP
									//case b0110: interpretation = "STC.L"; break; //0100nnnn01100011 - DSP
									//case b0111: interpretation = "STC.L"; break; //0100nnnn01110011 - DSP
									case b1000: interpretation = "STC.L R0_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn10000011
									case b1001: interpretation = "STC.L R1_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn10010011
									case b1010: interpretation = "STC.L R2_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn10100011
									case b1011: interpretation = "STC.L R3_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn10110011
									case b1100: interpretation = "STC.L R4_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn11000011
									case b1101: interpretation = "STC.L R5_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn11010011
									case b1110: interpretation = "STC.L R6_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn11100011
									case b1111: interpretation = "STC.L R7_BANK, @-R" + QString::number(nibble[1]); break; //0100nnnn11110011
								} break;
							case b0101:
								switch(nibble[2]) {
									case b0000: interpretation = "ROTR R" + QString::number(nibble[1]); break; //0100nnnn00000101
									case b0001: interpretation = "CMP/PL R" + QString::number(nibble[1]); break; //0100nnnn00010101
									case b0010: interpretation = "ROTCR R" + QString::number(nibble[1]); break; //0100nnnn00100101
								} break;
							case b1000:
								switch(nibble[2]) {
									case b0000: interpretation = "SHLL2 R" + QString::number(nibble[1]); break; //0100nnnn00001000
									case b0001: interpretation = "SHLL8 R" + QString::number(nibble[1]); break; //0100nnnn00011000
									case b0010: interpretation = "SHLL16 R" + QString::number(nibble[1]); break; //0100nnnn00101000
								} break;
							case b1001:
								switch(nibble[2]) {
									case b0000: interpretation = "SHLR2 R" + QString::number(nibble[1]); break; //0100nnnn00001001
									case b0001: interpretation = "SHLR8 R" + QString::number(nibble[1]); break; //0100nnnn00011001
									case b0010: interpretation = "SHLR16 R" + QString::number(nibble[1]); break; //0100nnnn00101001
								} break;
							case b1011:
								switch(nibble[2]) {
									case b0000: interpretation = "JSR @R" + QString::number(nibble[1]); break; //0100nnnn00001011
									case b0001: interpretation = "TAS.B @R" + QString::number(nibble[1]); break; //0100nnnn00011011
									case b0010: interpretation = "JMP @R" + QString::number(nibble[1]);	break;
								} break;
							case b1100: interpretation = "SHAD R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0100nnnnmmmm1100
							case b1101: interpretation = "SHLD R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0100nnnnmmmm1101
							case b1111: interpretation = "MAC.W @R" + QString::number(nibble[2]) + "+, @R" + QString::number(nibble[1]) + "+"; break; //0100nnnnmmmm1111
						} break;
					case b0101: interpretation = "MOV.L R" + QString::number(nibble[2]) + ",@(H'" + QString::number(4*nibble[3],16) + ",R" + QString::number(nibble[1]) + ")"; break; //0101nnnnmmmmdddd
					case b0110:
						switch(nibble[3]) {
							case b0000: interpretation = "MOV.B @R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm0000
							case b0001: interpretation = "MOV.W @R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm0001
							case b0010: interpretation = "MOV.L @R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm0010

							case b0011: interpretation = "MOV R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm0011
							case b0100: interpretation = "MOV.B @R" + QString::number(nibble[2]) + "+, R" + QString::number(nibble[1]); break; //0110nnnnmmmm0100
							case b0101: interpretation = "MOV.W @R" + QString::number(nibble[2]) + "+, R" + QString::number(nibble[1]); break; //0110nnnnmmmm0101
							case b0110: interpretation = "MOV.L @R" + QString::number(nibble[2]) + "+, R" + QString::number(nibble[1]); break; //0110nnnnmmmm0110
							case b0111: interpretation = "NOT R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm0111
							case b1000: interpretation = "SWAP.B R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1000
							case b1001: interpretation = "SWAP.W R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1001
							case b1010: interpretation = "NEGC R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1010
							case b1011: interpretation = "NEG R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1011
							case b1100: interpretation = "EXTU.B R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1100
							case b1101: interpretation = "EXTU.W R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1101
							case b1110: interpretation = "EXTS.B R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1110
							case b1111: interpretation = "EXTS.W R" + QString::number(nibble[2]) + ", R" + QString::number(nibble[1]); break; //0110nnnnmmmm1111
						} break;
					case b0111: interpretation = "ADD #H'" + QString::number(buffer[1],16).right(2) + ", R" + QString::number(nibble[1]); break; //0111nnnniiiiiiii
					case b1000:
						switch(nibble[1]) {
							case b0000: interpretation = "MOV.B R0, @(H'" + QString::number(nibble[3],16) + ",R" + QString::number(nibble[2]) + ")"; break; //10000000nnnndddd
							case b0001: interpretation = "MOV.W R0, @(H'" + QString::number(nibble[3],16) + ",R" + QString::number(nibble[2]) + ")"; break; //10000001nnnndddd
							//case b0010: interpretation = "SETRC"; break; //10000010iiiiiiii - DSP
							case b0100: interpretation = "MOV.B @(H'" + QString::number(nibble[3],16) + ",R" + QString::number(nibble[2]) + "),R0"; break; //10000100mmmmdddd
							case b0101: interpretation = "MOV.W @(H'" + QString::number(nibble[3],16) + ",R" + QString::number(nibble[2]) + "),R0"; break; //10000101mmmmdddd
							case b1000: interpretation = "CMP/EQ #H'" + QString::number(buffer[1],16) + ", R0"; break; //10001000iiiiiiii
							case b1001: interpretation = "BT H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) ; break; //10001001dddddddd
							case b1011: interpretation = "BF H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) ; break; //10001011dddddddd
							//case b1100: interpretation = "LDRS"; break; //10001100dddddddd - DSP
							case b1101: interpretation = "BT/S H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) ; break; //10001101dddddddd
							//case b1110: interpretation = "LDRE"; break; //10001110dddddddd - DSP
							case b1111: interpretation = "BF/S H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) ; break; //10001111dddddddd
						} break;
					case b1001: interpretation = "MOV.W @(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",PC),R" + QString::number(nibble[1]); break; //1001nnnndddddddd
					case b1010: interpretation = "BRA H'" + QString::number(2*((nibble[1]<<8)+(nibble[2]<<4)+nibble[3]),16) ; break; //1010dddddddddddd
					case b1011: interpretation = "BSR H'" + QString::number(2*((nibble[1]<<8)+(nibble[2]<<4)+nibble[3]),16) ; break; //1011dddddddddddd
					case b1100:
						switch(nibble[1]) {
							case b0000: interpretation = "MOV.B R0,@(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR)"; break; //11000000dddddddd
							case b0001: interpretation = "MOV.W R0,@(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR)"; break; //11000001dddddddd
							case b0010: interpretation = "MOV.L R0,@(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR)"; break; //11000010dddddddd
							case b0011: interpretation = "TRAPA #H'" + QString::number(buffer[1],16); break; //11000011iiiiiiii
							case b0100: interpretation = "MOV.B @(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR),R0"; break; //11000100dddddddd
							case b0101: interpretation = "MOV.W @(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR),R0"; break; //11000101dddddddd
							case b0110: interpretation = "MOV.L @(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",GBR),R0"; break; //11000110dddddddd
							case b0111: interpretation = "MOVA @(H'" + QString::number(2*((nibble[2]<<4)+nibble[3]),16) + ",PC),R0"; break; //11000111dddddddd
							case b1000: interpretation = "TST #H'" + QString::number(buffer[1],16) + ",R0"; break; //11001000iiiiiiii
							case b1001: interpretation = "AND #H'" + QString::number(buffer[1],16) + ",R0"; break; //11001001iiiiiiii
							case b1010: interpretation = "XOR #H'" + QString::number(buffer[1],16) + ",R0"; break; //11001010iiiiiiii
							case b1011: interpretation = "OR #H'" + QString::number(buffer[1],16) + ",R0"; break; //11001011iiiiiiii
							case b1100: interpretation = "TST.B #H'" + QString::number(buffer[1],16) + ",@(R0,GBR)"; break; //11001100iiiiiiii
							case b1101: interpretation = "AND.B #H'" + QString::number(buffer[1],16) + ",@(R0,GBR)"; break; //11001101iiiiiiii
							case b1110: interpretation = "XOR.B #H'" + QString::number(buffer[1],16) + ",@(R0,GBR)"; break; //11001110iiiiiiii
							case b1111: interpretation = "OR.B #H'" + QString::number(buffer[1],16) + ",@(R0,GBR)"; break; //11001111iiiiiiii
						} break;
					case b1101: interpretation = "MOV.L @(H'" + QString::number(4*((nibble[2]<<4)+nibble[3]),16) + ",PC), R" + QString::number(nibble[1]); break; //1101nnnndddddddd
					case b1110: interpretation = "MOV #H'" + QString::number(buffer[1],16) + ", R" + QString::number(nibble[1]); break; //1110nnnniiiiiiii
					/* SH-3E Only
					case b1111:
						switch(nibble[3]) {
							case b1101:
								switch(nibble[2]) {
									case b0000: interpretation = "FSTS"; break; //1111nnnn00001101
									case b0001: interpretation = "FLDS"; break; //1111nnnn00011101
									case b0010: interpretation = "FLOAT"; break; //1111nnnn00101101
									case b0011: interpretation = "FTRC"; break; //1111nnnn00111101
									case b0100: interpretation = "FNEG"; break; //1111nnnn01001101
									case b0101: interpretation = "FABS"; break; //1111nnnn01011101
									case b0110: interpretation = "FSQRT"; break; //1111nnnn01101101
									case b1000: interpretation = "1111nnnn10001101      FLDI0";	break;
									case b1001: interpretation = "1111nnnn10011101      FLDI1";	break;
								} break;
							case b0000: interpretation = "FADD"; break; //1111nnnnmmmm0000
							case b0001: interpretation = "FSUB"; break; //1111nnnnmmmm0001
							case b0010: interpretation = "FMUL"; break; //1111nnnnmmmm0010
							case b0011: interpretation = "FDIV"; break; //1111nnnnmmmm0011
							case b0100: interpretation = "FCMP/EQ"; break; //1111nnnnmmmm0100
							case b0101: interpretation = "FCMP/GT"; break; //1111nnnnmmmm0101
							case b0110: interpretation = "FMOV.S"; break; //1111nnnnmmmm0110
							case b0111: interpretation = "FMOV.S"; break; //1111nnnnmmmm0111
							case b1000: interpretation = "FMOV.S"; break; //1111nnnnmmmm1000
							case b1001: interpretation = "1111nnnnmmmm1001      FMOV.S";break;
							case b1010: interpretation = "FMOV.S"; break; //1111nnnnmmmm1010
							case b1011: interpretation = "FMOV.S"; break; //1111nnnnmmmm1011
							case b1100: interpretation = "FMOV"; break; //1111nnnnmmmm1100
							case b1110: interpretation = "FMAC"; break; //1111nnnnmmmm1110
						} break;*/
				}
			//Ajout de la ligne
				mainWindow->addInstruction(QString::number((0x300200+ligne),16),QString::number(nibble[0],16) + QString::number(nibble[1],16) + QString::number(nibble[2],16) + QString::number(nibble[3],16),interpretation);
				ligne+=2;
			//initialisation de la première ligne
				NextInstruction = mainWindow->setActiveLine("300200");
				mainWindow->setRegister(tr("PC"), tr("300200"));
			}
		}
		else
	   {

		   mainWindow->setStatus("Fichier non reconnu.",5000);
	   }
	}
	else
	{
		mainWindow->setStatus("Erreur de lecture du fichier.",5000);
	}
}
