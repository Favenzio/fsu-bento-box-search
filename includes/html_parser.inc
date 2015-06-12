<?php

/**
 * A Collection of functions that help parse HTML pages
 */

/**
 * Returns the processed html from a queried system.
 *
 * @nid
 *   the nid for the Search Source node
 * 
 * @keywords
 *   the keywords to use when querying the system
 * 
 * @return
 *   A result set object containing the list of search sources.
 */
function _process_system_query($nid, $keywords) {
  //get the search source node object
  $search_source = node_load($nid);
  //get the fields attached to the node
  $results_container_field = $search_source->results_container;
  $single_result_field = $search_source->single_result;
  //retrieve the values given by the user
  $results_container = $results_container_field['und'][0]['value'];
  $single_result = $single_result_field['und'][0]['value'];
  //replace the keywords from the search into the search url
  $processed_source_url = _url_replace_keywords($nid, $keywords);

  //call the html parsing function
  $processed_html = _bento_search_html_parser($processed_source_url, 
    $results_container, $single_result);

  return $processed_html;
}

/**
 * Retrieves the Search Source node and replaces 
 * the [keywords] section of the search_source_url with the @keywords argument
 *
 */
function _url_replace_keywords($nid, $keywords) {
  //get the search source node object
  $search_source = node_load($nid);
  $search_source_url_field = $search_source->search_source_url;
  $clean_source_url = $search_source_url_field['und'][0]['value'];
  //replace the keywords from the search into the search url
  $processed_source_url = str_replace('[keywords]', $keywords, $clean_source_url);

  return $processed_source_url;
}

/**
 * Retrieves a url and parses it according to the parameters given.
 *
 * @source_url
 *   the url for the system to be queried
 * 
 * @results_container
 *   the ID or Class of the element surrounding the results
 * 
 * @single_result
 *   The ID or Class of a single result element
 */
function _bento_search_html_parser($source_url, $results_container, $single_result) {
  $processed_html = '';
  $raw_html = file_get_contents($source_url);  
  $dom = new domDocument;
  $dom->loadHTML($raw_html); //load the html into an object 
  $dom->preserveWhiteSpace = false; //discard white space
  
  //get the html in the container specified if it is surrounded by a class
  if($results_container[0] == '.') {
    $results_array = _get_html_by_class($dom, $results_container);
    foreach ($results_array as $result) {
      $processed_html .= $result;
    }
  }
  
  //get the html in the container specified if it is surrounded by an id
  if($results_container[0] == '#') {
    $results_array = _get_html_by_id($dom, $results_container);
    for ($x = 0; $x < MAX_RESULT_COUNT; $x++) {
      $processed_html .= $results_array[$x];
    }
  }
  
  $relative_html = _parse_single_results($processed_html, $single_result);
  $final_html = _change_relative_to_absolute_url($relative_html, $source_url);
  
  return $final_html;
}

/**
 * Returns an array of individual results based on the ID or Class passed in
 * the @single_result_marker parameter.
 */
function _parse_single_results($html, $single_result_marker) {
  $processed_html = '';
  $dom = new domDocument;
  $dom->loadXML($html); //load the html into an object 
  $dom->preserveWhiteSpace = false; //discard white space
  
  //remove the class and id attributes from the container
  $dom->documentElement->removeAttribute('class');
  $dom->documentElement->removeAttribute('id');
  
  //get the html in the single result specified if it is surrounded by a class
  if($single_result_marker[0] == '.') {
    $results_array = _get_html_by_class($dom, $single_result_marker);
    for ($x = 0; $x < MAX_RESULT_COUNT; $x++) {
      $processed_html .= $results_array[$x];
    }
  }
  //get the html in the single result specified if it is surrounded by an id
  elseif($single_result_marker[0] == '#') {
    $results_array = _get_html_by_id($dom, $single_result_marker);
    for ($x = 0; $x < MAX_RESULT_COUNT; $x++) {
      $processed_html .= $results_array[$x];
    }
  } 
  else {
    $results_array = _get_html_by_element($dom, $single_result_marker);
    for ($x = 0; $x < MAX_RESULT_COUNT; $x++) {
      $processed_html .= $results_array[$x];
    }
  }
  
  return $processed_html;
}

/**
 * Returns an array of HTML snippets found using 
 * the element specified by the @class argument
 */
function _get_html_by_class ($dom, $class) {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $classname = substr($class, 1);
  $xpath_results = 
    $xpath->query("//*[contains(@class,'$classname')]");
    
  foreach($xpath_results as $xpath_result) {
    $html_results[] = $dom->saveXML($xpath_result);
  }
  
  return $html_results;
}

/**
 * Returns an array of HTML snippets found using 
 * the element specified by the @id argument
 */
function _get_html_by_id ($dom, $id) {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $idname = substr($id, 1);
  $xpath_results = 
    $xpath->query("//*[contains(@id,'$idname')]");
    
  foreach($xpath_results as $xpath_result) {
    $html_results[] = $dom->saveXML($xpath_result);
  }
  
  return $html_results;
}

/**
 * Returns an array of HTML snippets found using 
 * the element specified by the @element argument
 */
function _get_html_by_element ($dom, $element) {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $xpath_query = "//" . $element;
  $xpath_results = $xpath->query($xpath_query);
    
  foreach($xpath_results as $xpath_result) {
    $html_results[] = $dom->saveXML($xpath_result);
  }
  
  return $html_results;
}

/**
 * Parses HTML and replaces any relative paths with absolute ones.
 * 
 * @relative_html is the HTML to be parsed.
 * @source_url the URL that was initially queried.
 * 
 */
function _change_relative_to_absolute_url($relative_html, $source_url) {
  $parsed_url = parse_url($source_url);
  $base_url = $parsed_url['scheme']."://".$parsed_url['host'];
  $full_url = '$1' . $base_url . $parsed_url['path'] . '$2$3';
  
  $absolute_html = 
    preg_replace("#(<\s*a\s+[^>]*href\s*=\s*[\"'])(?!http)([^\"'>]+)([\"'>]+)#", 
    $full_url, $relative_html);
  
  return $absolute_html;
}