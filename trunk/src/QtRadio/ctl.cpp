/* File:   ctl.cpp
 * Author: Graeme Jury, ZL2APV
 *
 * Created on 16 September 2011, 17:34
 */

/* Copyright (C)
* 2011 - Graeme Jury, ZL2APV
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
#include "ctl.h"
#include "ui_ctl.h"
#include <QDebug>

Ctl::Ctl(QWidget *parent):QFrame(parent), ui(new Ui::Ctl)
{
    ui->setupUi(this);
    micBar = ui->MicProgressBar;

    moxPwr = 80;
    TunePwr = 50;
    audioGain = 100;
    ui->audioSlider->setValue(audioGain);
    ui->pwrSlider->setValue(moxPwr);
    ui->tunePwrSlider->setValue(TunePwr);

    HideTX(false); // Hide buttons because we have not connected to anything yet
    connect(this, SIGNAL(audioGainInitalized(int)), this, SLOT(setAudioSlider(int)));
    connect(this, SIGNAL(setAudioMuted(bool)), this, SLOT(setAudioMute(bool)));
}

Ctl::~Ctl()
{
    delete ui;
}

void Ctl::on_btnMox_clicked(bool checked)
{
    bool ptt;

    ui->btnTune->setChecked(false); //Override the tune button
    if (checked)
    { //We are going from Rx to Tx
        ui->btnMox->setChecked(true);
        ptt = true;
        ui->pwrSlider->setValue(moxPwr);
    }
    else
    {
        ui->btnMox->setChecked(false);
        ptt = false;
        ui->MicProgressBar->setValue(0);
    }
    emit pttChange(0, ptt);
} // end on_btnMox_clicked


void Ctl::on_btnTune_clicked(bool checked)
{
    bool ptt;

    ui->btnMox->setChecked(false); //Override the MOX button
    if(checked)
    { //We are going from Rx to Tx
        ui->btnTune->setChecked(true);
        ptt = true;
        ui->tunePwrSlider->setValue(TunePwr);
    }
    else
    {
        ui->btnTune->setChecked(false);
        ptt = false;
    }
    emit pttChange(1, ptt);
} // end on_btnTune_clicked


void Ctl::on_pwrSlider_valueChanged(int value)
{
    moxPwr = ui->pwrSlider->value(); //Do nothing until Tx power level adj written for DttSP
    emit pwrSlider_valueChanged(((double)moxPwr)/1000); //KD0OSS
} // end on_pwrSlider_valueChanged


double Ctl::getTxPwr()
{
    return (double)ui->pwrSlider->value()/1000;
} // end getTxPwr


void Ctl::on_tunePwrSlider_valueChanged(int value)
{
    TunePwr = ui->tunePwrSlider->value();
    emit tunePwrSlider_valueChanged(((double)TunePwr)/1000); //KD0OSS
} // end on_tunePwrSlider_valueChanged


double Ctl::getTunePwr()
{
    return (double)ui->tunePwrSlider->value()/1000;
} // end getTunePwr


void Ctl::update_mic_level(qreal level)
{
    ui->MicProgressBar->setValue(100*level);
} // end update_mic_level


void Ctl::clearMoxBtn()
{
    ui->btnMox->setChecked(false);
    ui->btnTune->setChecked(false);
} // end clearMoxBtn


void Ctl::HideTX(bool cantx)
{
    if (cantx)
    {
        ui->btnMox->setEnabled(true);
        ui->btnTune->setEnabled(true);
    }
    else
    {
        clearMoxBtn();
        ui->btnMox->setEnabled(false);
        ui->btnTune->setEnabled(false);
    }
} // end HideTX


void Ctl::RigCtlTX(bool rigctlptt)
{
    if (rigctlptt && ui->btnMox->isEnabled())
        on_btnMox_clicked(true);
    else
        on_btnMox_clicked(false);
} // end RigCtlTX


void Ctl::loadSettings(QSettings *settings)
{
    settings->beginGroup("Ctl");
    moxPwr = settings->value("moxPwr",50).toInt();
    TunePwr = settings->value("tunePwr",1).toInt();
    settings->endGroup();
    ui->pwrSlider->setValue(moxPwr); // KD0OSS
    ui->tunePwrSlider->setValue(TunePwr); // KD0OSS
} // end loadSettings


void Ctl::saveSettings(QSettings *settings)
{
    settings->beginGroup("Ctl");
    settings->setValue("moxPwr", moxPwr);
    settings->setValue("tunePwr", TunePwr);
    settings->endGroup();
} // end saveSettings


void Ctl::on_btnMaster_clicked()
{
    emit masterBtnClicked();
} // end on_btnMaster_clicked


void Ctl::on_btnMute_clicked(bool checked)
{
    ui->audioSlider->setEnabled(!checked);
    emit audioMuted(checked);
} // end on_btnMute_clicked


void Ctl::on_audioSlider_valueChanged(int value)
{
    audioGain = value;
    emit audioGainChanged();
} // end on_audioSlider_valueChanged


void Ctl::setAudioSlider(int gain)
{
    ui->audioSlider->setValue(gain);
} //end setAudioSlider


void Ctl::setAudioMute(bool muted)
{
    ui->btnMute->setChecked(muted);
    ui->audioSlider->setEnabled(!muted);
} // end setAudioMute
