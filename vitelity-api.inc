<?php

/**
 * Vitelity API PHP Class
 * Based on: http://api.vitelity.net/apihelp.html
 * Written by Peter Beckman <beckman@angryox.com>
 * August 8, 2006
 * 
 * This work is licensed under a 
 * Creative Commons Attribution-Noncommercial-Share Alike 3.0 United States License.
 *   http://creativecommons.org/licenses/by-nc-sa/3.0/us/
 *
 * Requires PEAR HTTP_Request Library, http://pear.php.net/package/HTTP_Request
 *
 * All functions return FALSE on failure, or on success, either:
 *
 *      1      the command returned "success"
 *      array  an array containing the list returned from the command
 *
 * If a failure occurs, $this->errormsg will be set with the reason it failed.
 *
 * USAGE:
 *
 *     require_once('vitelity-api.inc');
 *     $v = new vitelityapi;
 *     $ratecenters = $v->listratecenters('VA');
 *
 */
require_once("HTTP/Request.php");

class vitelityapi {

    /**
     * If testmode is on, returns success for API functions that would 
     * charge your account or change your account in any way.  The functions
     * that will not actually execute but return success are:
     *      gettollfree, getlocaldid, removedid, requestvanity, localbackorder, 
     *      reroute, routeall
     */

    var $testmode = 0;
    var $debug    = 0;
    var $login    = '';
    var $pass     = '';
    var $url      = 'http://api.vitelity.net/api.php';
    var $r        = '';
    var $params   = '';
    var $maxretries = 3; // Number of times to retry the HTTP request if it fails
            
    function listratecenters($state) {
        return $this->_handleQuery(array(
            'cmd'   => __FUNCTION__,
            'state' => $state
        ));
    }

    function listavailratecenters($state) {
        return $this->_handleQuery(array(
            'cmd'   => __FUNCTION__,
            'state' => $state
        ));
    }

    function getlocaldid($did, $type='perminute') {
        return $this->_handleQuery(array(
            'cmd'  => __FUNCTION__, 
            'did'  => $did,
            'type' => $type
        ));
    }

    function removedid($did) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__, 
            'did' => $did
        ));
    }

    function listlocal($state, $ratecenter='', $withrates='no', $type='perminute') {
        return $this->_handleQuery(array(
            'cmd'        => __FUNCTION__, 
            'state'      => $state,
            'ratecenter' => $ratecenter,
            'withrates'  => $withrates,
            'type'       => $type
        ));
    }

    function localbackorder($state, $ratecenter, $type='perminute') {
        return $this->_handleQuery(array(
            'cmd'        => __FUNCTION__, 
            'state'      => $state,
            'ratecenter' => $ratecenter,
            'type'       => $type
        ));
    }

    function listtollfree() {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__
        ));
    }

    function gettollfree($did) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__, 
            'did' => $did
        ));
    }

    function searchtoll($did) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__, 
            'did' => $did
        ));
    }

    function requestvanity($did) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__, 
            'did' => $did
        ));
    }

    function listavailstates($type='perminute') {
        return $this->_handleQuery(array(
            'cmd'  => __FUNCTION__,
            'type' => $type
        ));
    }

    function liststates($type='perminute') {
        return $this->_handleQuery(array(
            'cmd'  => __FUNCTION__,
            'type' => $type
        ));
    }

    function cnam($did) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__,
            'did' => $did
        ));
    }

    function reroute($did, $routesip) {
        return $this->_handleQuery(array(
            'cmd'      =>__FUNCTION__,
            'did'      =>$did,
            'routesip' =>$routesip
        ));
    }

    function routeall($routesip) {
        return $this->_handleQuery(array(
            'cmd'      =>__FUNCTION__,
            'routesip' =>$routesip
        ));
    }

    function balance() {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__
        ));
    }

    function listdids($extra='yes') {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__,
            'extra' => $extra
        ));
    }

    function getrate($number) {
        return $this->_handleQuery(array(
            'cmd'    => __FUNCTION__,
            'number' => $number
        ));
    }

    function subaccounts($do) {
        return $this->_handleQuery(array(
            'cmd' => __FUNCTION__,
            'do'  => $do
        ));
    }

    function callfromclick($callbacknumber, $calltonumber) {
        return $this->_handleQuery(array(
            'cmd'           => __FUNCTION__,
            'number'        => $callbacknumber,
            'servicenumber' => $calltonumber
        ));
    }


    /***************************************
     * These functions are private; you    *
     * can access them, but you shouldn't. *
     ***************************************/

    function _handleQuery($params) {
        $this->params = $params;
        $testfunctions = array('gettollfree', 'getlocaldid', 'removedid', 'requestvanity', 'localbackorder', 'reroute',
            'routeall');
        if ($this->testmode and in_array($params['cmd'], $testfunctions)) {
            $this->_log("Testmode Active, returning 1 for {$params['cmd']}");
            return 1;
        }

        $r =& new HTTP_Request($this->url);
        $r->setMethod(HTTP_REQUEST_METHOD_POST);
        $r->addPostData('login',$this->login); 
        $r->addPostData('pass',$this->pass); 

        foreach($params as $k => $v) {
            $r->addPostData($k, $v);
        }

        for ($i=0;$i<$this->maxretries;$i++) {
            $reqReturn = $r->sendRequest();
            if (!PEAR::isError($reqReturn)) {
                if ($this->debug) { $this->_log("Vitelity Parameters: " . print_r($params,1)); }
                $response = $this->_parseResponse($r->getResponseBody());
                if ($this->debug) { $this->_log("Got Response from Vitelity: " . print_r($response,1)); }
                return $response;
            }
        }

        /**
         *  If we get here, the request has failed $this->maxretries times.
         */
        $this->errorstr = "HTTP Request failed: ".$reqReturn->getMessage() . "\nParams:" . print_r($params,1);
        $this->_log($this->errorstr, 'error');
        return FALSE;
    }

    function _parseResponse($data) {
        if ($this->debug) { $this->_log("Got HTTP Response: $data"); }
        $errors = array(
            'missingdata' => 'Missing login, pass or cmd parameters in URL',
            'invalidauth' => 'Invalid username or password',
            'invalid'     => 'Invalid parameters?',
            'missingrc'   => 'Missing or invalid state or ratecenter',
            'unavailable' => 'Requested Number Unavailable',
            'none'        => 'No Toll Free Numbers Available',
            'missingdid'  => 'Missing did parameter',
        );

        $ret = explode("[[", $data);
        if (!isset($ret[1])) {
            $this->errorstr = "Error in return Data: $data";
            $this->_log($this->errorstr, 'error');
            return FALSE;
        }

        $res = trim($ret[1]);

        if (isset($errors[$res])) {
            $this->errorstr = $errors[$res];
            $this->_log("Got Non-Success Response: {$this->errorstr} params: " . print_r($this->params, 1), 'info');
            return FALSE;
        } else {
            if ($res == 'success') {
                if ($this->debug) { $this->_log("Success!"); }
                return 1;
            } else { 
                if ($this->debug) { $this->_log("Returned: $res"); }
                $numlist = explode("\n", $res);
                /*
                 * Sometimes AT&T returns letters instead of numbers.  We normalize.
                 */
                $patterns = array('/[a-cA-C]/','/[d-fD-F]/','/[g-iG-I]/','/[j-lJ-L]/',
                                  '/[m-oM-O]/','/[p-sP-S]/','/[t-vT-V]/','/[w-zW-Z]/');
                $replacements = array(2,3,4,5,6,7,8,9);
                foreach($numlist as $k => $v) {
                    if (strlen($v) == 10 and is_numeric(substr($v,0,3))) {
                        $numlist[$k] = preg_replace($patterns, $replacements, $v);
                    }
                }
                return $numlist;
            }
        }
    }

    /**
     * Pseudo logging functionality. Override this in your own implementation to get logging.
     * For example:
     *   class vitelityapiextended extends vitelityapi {
     *     function _log($msg, $level='debug') {
     *         echo $msg."\n";
     *         //  If you want to get the default logging too, keep this line
     *         parent::_log($msg, $level);
     *      }
     *   }
     * 
     * @param string $msg   The log message
     * @param string $level Syslog log levels: 
     *                        debug, info, notice, warning, 
     *                        err, error, crit, critical, 
     *                        alert, emerg, emergency
     * @param boolean       Returns TRUE
     */
    function _log($msg, $level='debug') {
        error_log($level . ': ' . $msg);
        return TRUE;
    }
        
}
