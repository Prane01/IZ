package com.teamcenter.model;

public class BAData {

    private String hbtNotiFreq = "";
    private String hbtName = "";
    private String certName = "";
    private String docType = "";
    private String docNum = "";
    private String affctdRegSpec = "";
    private String timeline = "";
    private String actnNeededBy = "";
    private String regOwners = "";
    private String expOrWithdDate = "";
    private String dateReleased = "";
    private String certAWCurl = "";
    private String affSku = "";
    private int skuCount = 0;

    public String getCertAWCurl() {
        return certAWCurl;
    }

    public void setCertAWCurl(String certAWCurl) {
        this.certAWCurl = certAWCurl;
    }

    public String getDateReleased() {
        return dateReleased;
    }

    public void setDateReleased(String dateReleased) {
        this.dateReleased = dateReleased;
    }

    public int getSkuCount() {
        return skuCount;
    }

    public void setSkuCount(int skuCount) {
        this.skuCount = skuCount;
    }

    public String getCertName() {
        return certName;
    }

    public void setCertName(String certName) {
        this.certName = certName;
    }

    public String getDocType() {
        return docType;
    }

    public void setDocType(String docType) {
        this.docType = docType;
    }

    public String getDocNum() {
        return docNum;
    }

    public void setDocNum(String docNum) {
        this.docNum = docNum;
    }

    public String getAffctdRegSpec() {
        return affctdRegSpec;
    }

    public void setAffctdRegSpec(String affctdRegSpec) {
        this.affctdRegSpec = affctdRegSpec;
    }

    public String getTimeline() {
        return timeline;
    }

    public void setTimeline(String timeline) {
        this.timeline = timeline;
    }

    public String getActnNeededBy() {
        return actnNeededBy;
    }

    public void setActnNeededBy(String actnNeededBy) {
        this.actnNeededBy = actnNeededBy;
    }

    public String getRegOwners() {
        return regOwners;
    }

    public void setRegOwners(String regOwners) {
        this.regOwners = regOwners;
    }

    public String getExpOrWithdDate() {
        return expOrWithdDate;
    }

    public void setExpOrWithdDate(String expOrWithdDate) {
        this.expOrWithdDate = expOrWithdDate;
    }

    public String getAffSku() {
        return affSku;
    }

    public void setAffSku(String affSku) {
        this.affSku = affSku;
    }

    public String getHbtName() {
        return hbtName;
    }

    public void setHbtName(String hbtName) {
        this.hbtName = hbtName;
    }

    public String getHbtNotiFreq() {
        return hbtNotiFreq;
    }

    public void setHbtNotiFreq(String hbtNotiFreq) {
        this.hbtNotiFreq = hbtNotiFreq;
    }

    @Override
    public String toString() {
        return "BAData{" +
                "certName='" + certName + '\'' +
                ", docType='" + docType + '\'' +
                ", docNum='" + docNum + '\'' +
                ", expOrWithdDate='" + expOrWithdDate + '\'' +
                ", dateReleased='" + dateReleased + '\'' +
                ", regOwners='" + regOwners + '\'' +
                ", skuCount=" + skuCount +
                '}';
    }
}
