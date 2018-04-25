clear;clc;
ctl.len=400;ctl.chart=5;
ctl.m=3;count=1;flag=1;
N93=ones(4,3);  X3 = ones(4,3,ctl.len); F42=ones(3,3);   C=4.5594e05; flag2=0; rho=0; arfa=0; beta=0; x=0; y=0; z=0;xfinal=0;yfinal=0;zfinal=0;psi=0;theta=0; phi=0; x_pre=0; y_pre=0; z_pre=0;
close('all');ctl.s = serial('COM3');%s = serial('/dev/tty.usbserial-141');\
ctl.s.BaudRate=256000;ctl.s.InputBufferSize=2048;ctl.nLine=0;nChart=5;
try
    fopen(ctl.s);
catch
    fclose(instrfind);
    fopen(ctl.s);
end
ctl.status=0;ctl.flag=0;
while(1)
    nBuffer = ctl.s.BytesAvailable
    while(1)
        ctl.rd= fread(ctl.s,1);
        switch ctl.status
            case 0
                if ctl.rd ==127;ctl.status=7;end
            case 7
                if ctl.rd ==146
                    data= fread(ctl.s,48);
                    if length(data) ~= 48
                        ctl.status = 0;break;
                    end
                    ctl.k=1;
                    for i = 1:4
                        for j =1:3
                            N93(i,j) = double(typecast(uint32( bitshift(data(ctl.k),24) + bitshift(data(ctl.k+1),16) + bitshift(data(ctl.k+2),8) + data(ctl.k+3)),'int32'))/100;
                            ctl.k = ctl.k+4;
                        end
                    end
                    %                     N93(4,:)=sqrt(sum(N93(1:3,:).^2));
                    N93(:,1)=N93(:,1)*6.892/4.228;   N93(:,2)=N93(:,2)*6.892/5.545;
                    F4=N93(1:3,:).*F42;
                    %X3 = cat(3,X3(:,:,2:end),[F4(1:2,:);[rho,arfa,beta];[x,y,z]]);
                    X3 = cat(3,X3(:,:,2:end),[F4(1:1,:);[rho,arfa,beta];[xfinal,yfinal,zfinal];[psi,theta,phi]]);
                    %X3 = cat(3,X3(:,:,2:end),[F4(1:3,:);[xfinal,yfinal,zfinal]]);
                    count=count+1;
                     %%%符号矩阵
                    if ((count>=500)&&(flag==0))
                    %预置点
                   % alpha=45/180*pi; beta=10/180*pi;
                    F4_Set=[1,1,-1;1,1,-1;-1,-1,-1]; 
                    for i =1:3
                        for j=1:3
                          if F4(i,j)*F4_Set(i,j)<0
                              F42(i,j)=-F42(i,j);
                          else
                               F42(i,j)=F42(i,j);
                          end 
                        end
                    end
                        flag=1;
                        flag2=1;
                    end
                
                    bx=F4(:,1);             by=F4(:,2);            bz=F4(:,3);
                    Px=sum(bx.^2);    Py=sum(by.^2);   Pz=sum(bz.^2);
                    
                    rho=(1.5*C^2/(Px+Py+Pz))^(1/6);%计算相对距离
                    A1=2/9*[5 -1 -1; -1 5 -1;-1 -1 5]*[Px;Py;Pz];
                    %A1=A1';
                    X=(rho^4/C)*sqrt(real(A1(1)));  Y=(rho^4/C)*sqrt(real(A1(2)));   Z=(rho^4/C)*sqrt(real(A1(3)));
                    fu12=bx'*by;  fu23=by'*bz; fu31=bz'*bx;
                    %根据内积值判断X,Y,Z坐标的正负号
                    if (fu12>0)&&(fu23<0)&&(fu31<0)
                        x=X; 
                        y=Y;
                        z=-Z; 
                        x2=-X;
                        y2=-Y;
                        z2=Z;
                    end
                    if (fu12<0)&&(fu23<0)&&(fu31>0)
                        x=-X;
                        y=Y;
                        z=-Z;
     
                        x2=X;
                        y2=-Y;
                        z2=Z;
                    end
                    if (fu12>0)&&(fu23>0)&&(fu31>0)
                        x=-X;
                        y=-Y;
                        z=-Z;

                        x2=X;
                        y2=Y;
                        z2=Z;
                    end
                    if (fu12<0)&&(fu23>0)&&(fu31<0)
                        x=X;
                        y=-Y;
                        z=-Z;

                        x2=-X;
                        y2=Y;
                        z2=Z;
                    end
                    %arfa betia 解算
                    F=(4*rho^6/C^2)*(F4'*F4);%得到算法中的F矩阵，data是传感器测量矩阵
                    f33=F(3,3); beta=asin((abs(f33-1)/3)^(1/2))/pi*180; %计算俯仰角beta
                    f11=F(1,1);f23=F(2,3);   f13=F(1,3);
                    arfa=atan2(-f23,-f13)/pi*180;
                    if (arfa<0)
                        arfa=arfa+360;
                    end
                    %姿态解算
                    arfa1=arfa/180*pi;
                    beta1=beta/180*pi;
                    Talpha_1=[cos(arfa1) sin(arfa1) 0;    %位置变换矩阵A
                        -sin(arfa1) cos(arfa1) 0;
                        0        0        1];
                    Tbeta_1=[cos(beta1) 0 -sin(beta1);
                        0        1        0;
                        sin(beta1) 0 cos(beta1)];   %位置变换矩阵B
                    Talpha_inv_1=[cos(-arfa1) sin(-arfa1) 0; -sin(-arfa1) cos(-arfa1) 0; 0 0 1];  %位置逆变换矩阵A
                    Tbeta_inv_1=[cos(-beta1) 0 -sin(-beta1); 0 1 0; sin(-beta1) 0 cos(-beta1)];
                    S_1=[1 0 0;0 -0.5 0;0 0 -0.5];  %S变换矩阵
                    f3_1=C/rho^3*Talpha_inv_1*Tbeta_inv_1*S_1*Tbeta_1*Talpha_1;
                    
                    A=F4*inv(f3_1);
                    psi=atan2(real(A(1,2)),real(A(1,1)))/pi*180;
                    if psi<0
                        psi=psi+360;
                    end
                    phi=atan2(real(A(2,3)),real(A(3,3)))/pi*180;
                    if phi<0
                        phi=phi+360;
                    end
                    theta=asin(-real(A(1,3)))/pi*180;
                    
                    if (flag2==1)
                     x_pre=x;
                     y_pre=y;
                     z_pre=z;
                     flag2=0;
                    end   
                    distance1=sqrt((x-x_pre)^2+(y-y_pre)^2+(z-z_pre)^2);
                    distance2=sqrt((x2-x_pre)^2+(y2-y_pre)^2+(z2-z_pre)^2);
                      if(distance1<distance2)
                         xfinal=x;
                         yfinal=y;
                         zfinal=z;
                      else
                         xfinal=x2;
                         yfinal=y2;
                         zfinal=z2;
                      end
                      
                      x_pre=xfinal;
                      y_pre=yfinal;
                      z_pre=zfinal;
                      %PlotPlane()

                    ctl.status = 0;ctl.flag = 1;ctl.nLine=ctl.nLine+1;
                    if mod(ctl.nLine,20)==0 && nBuffer < 2000
                        for i =1:4
                            for j=1:3
                                subplot(4,3,(i-1)*3+j);plot(1:ctl.len,medfilt1(squeeze(X3(i,j,:)),3),'b');         
                            end
                        end
                        drawnow;
                    end%mod if
                end
                ctl.status=0;
            otherwise disp('other value');
        end%switch
        if ctl.flag ~=0; ctl.flag =0;break;end
    end %while(1)
end%for