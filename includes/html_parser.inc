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
  $element_replace_field = $search_source->element_replace;
  $element_suppress_field = $search_source->element_suppress;
  $link_element_field = $search_source->link_element;
  $search_source_format_field = $search_source->search_source_format;
  $search_source_url_field = $search_source->search_source_url;
  $more_results_url_field = $search_source->more_results_url;
  
  //retrieve the values given by the user
  $results_container = $results_container_field['und'][0]['value'];
  $single_result = $single_result_field['und'][0]['value'];
  $elements_to_replace = $element_replace_field['und'][0]['value'];
  $elements_to_suppress = $element_suppress_field['und'][0]['value'];
  $link_element = $link_element_field['und'][0]['value'];
  $search_source_format = $search_source_format_field['und'][0]['value'];
  $search_source_url = $search_source_url_field['und'][0]['value'];
  $more_results_url = $more_results_url_field['und'][0]['value'];
  
  //replace the keywords from the search into the search url
  $processed_source_url = _url_replace_keywords($search_source_url, $keywords);

  //retrieve the URL for more results
  $processed_more_results_url = _url_replace_keywords($more_results_url, $keywords);
  
  //call the html parsing function
  $processed_html = _bento_search_html_parser($processed_source_url, $results_container, 
    $single_result, $link_element, $elements_to_replace, $elements_to_suppress, $search_source_format);

  $more_results_text = '<p id="more-results"><a href="' . $processed_more_results_url . 
    '" target="_blank">See More Results...</a>';
  
  return $processed_html . $more_results_text;
}

/**
 * Retrieves the Search Source node and replaces 
 * the [keywords] section of the @url with the @keywords argument
 *
 */
function _url_replace_keywords($url, $keywords) {
  
  //replace the keywords from the search into the search url
  $processed_url = str_replace('[keywords]', $keywords, $url);
  $final_url = str_replace(' ', '%20', trim($processed_url));

  return $final_url;
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
 * 
 * @elements_to_replace
 *   XML elements that need to be replaced for display
 */
function _bento_search_html_parser($source_url, $results_container, $single_result, 
  $link_element, $elements_to_replace, $elements_to_suppress, $search_source_format) {
  
  $processed_html = '';
  $raw_xml = file_get_contents($source_url);
  $dom = new domDocument;
  
  if($search_source_format) {
    $dom->loadXML($raw_xml);
  } else {
    $dom->loadHTML($raw_xml);
  }
  $dom->preserveWhiteSpace = false; //discard white space
  
  //get the html in the container specified if it is surrounded by a class
  if($results_container[0] == '.') {
    $results_array = _get_html_by_class($dom, $results_container, FALSE);
    foreach ($results_array as $result) {
      $processed_html .= $result;
    }
  }
  //get the html in the container specified if it is surrounded by an id
  elseif($results_container[0] == '#') {
    $results_array = _get_html_by_id($dom, $results_container, FALSE);
    foreach ($results_array as $result) {
      $processed_html .= $result;
    }
  }
  else {
    $results_array = _get_html_by_element($dom, $results_container, FALSE);
    foreach ($results_array as $result) {
      $processed_html .= $result;
    }
  }
  
  $relative_html = _parse_single_results($processed_html, $single_result, $link_element,
    $search_source_format, $elements_to_suppress);
  $replaced_html = _replace_elements($relative_html, $elements_to_replace);
  $final_html = _change_relative_to_absolute_url($replaced_html, $source_url);
  
  return $final_html;
}

/**
 * Returns an array of individual results based on the ID or Class passed in
 * the @single_result_marker parameter.
 */
function _parse_single_results($html, $single_result_marker, $link_element, $search_source_format, $elements_to_suppress) {
  $processed_html = '';
  $dom = new domDocument;
  
  if($search_source_format) {
    $dom->loadXML($html);
  } else {
    $dom->loadHTML($html);
  }
  
  $dom->preserveWhiteSpace = false; //discard white space
  
  //remove the class and id attributes from the container
  if(isset($dom->documentElement)) {
    $dom->documentElement->removeAttribute('class');
    $dom->documentElement->removeAttribute('id');
  }
  else {
    return ZERO_RESULT_MESSAGE;
  }
  //get the html in the single result specified if it is surrounded by a class
  if($single_result_marker[0] == '.') {
    $results_array = _get_html_by_class($dom, $single_result_marker, TRUE, $link_element, $elements_to_suppress);
    for ($x = 0; $x < MAX_RESULT_COUNT and $x < count($results_array); $x++) {
      $processed_html .= '<div class="bento-single-result">' . $results_array[$x] . '</div>';
    }
  }
  //get the html in the single result specified if it is surrounded by an id
  elseif($single_result_marker[0] == '#') {
    $results_array = _get_html_by_id($dom, $single_result_marker, TRUE, $link_element, $elements_to_suppress);
    for ($x = 0; $x < MAX_RESULT_COUNT and $x < count($results_array); $x++) {
      $processed_html .= '<div class="bento-single-result">' . $results_array[$x] . '</div>';
    }
  } 
  else {
    $results_array = _get_html_by_element($dom, $single_result_marker, TRUE, $link_element, $elements_to_suppress);
    for ($x = 0; $x < MAX_RESULT_COUNT and $x < count($results_array); $x++) {
      $processed_html .= '<div class="bento-single-result">' . $results_array[$x] . '</div>';
    }
  }
  
  return $processed_html;
}

/**
 * Returns an array of XML snippets found using 
 * the element specified by the @class argument
 */
function _get_html_by_class ($dom, $class, $individual_result, $link_element = '', $elements_to_suppress = '') {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $classname = substr($class, 1);
  $xpath_results = 
    $xpath->query("//*[contains(@class,'$classname')]");
    
  foreach($xpath_results as $xpath_result) {
    
    if($individual_result) {
      $xpath_result = _add_href_to_first_element($xpath_result, $link_element);
      $xpath_result = _suppress_elements($xpath_result, $elements_to_suppress);
    }
    
    $html_results[] = $dom->saveXML($xpath_result);
  }
  
  return $html_results;
}

/**
 * Returns an array of XML snippets found using 
 * the element specified by the @id argument
 */
function _get_html_by_id ($dom, $id, $individual_result, $link_element = '', $elements_to_suppress = '') {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $idname = substr($id, 1);
  $xpath_results = 
    $xpath->query("//*[contains(@id,'$idname')]");
    
  foreach($xpath_results as $xpath_result) {
    
    if($individual_result) {
     $xpath_result = _add_href_to_first_element($xpath_result, $link_element);
     $xpath_result = _suppress_elements($xpath_result, $elements_to_suppress);
    }
    
    $html_results[] = $dom->saveXML($xpath_result);
  }
  
  return $html_results;
}

/**
 * Returns an array of XML snippets found using 
 * the element specified by the @element argument
 */
function _get_html_by_element ($dom, $element, $individual_result, $link_element = '', $elements_to_suppress = '') {
  $html_results = array();
  $xpath = new DOMXPath($dom);
  $xpath_query = "//" . $element;
  $xpath_results = $xpath->query($xpath_query);
    
  foreach($xpath_results as $xpath_result) {
    
    if($individual_result) {
      $xpath_result = _add_href_to_first_element($xpath_result, $link_element);
      $xpath_result = _suppress_elements($xpath_result, $elements_to_suppress);
    }
    
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
  $full_url = '$1' . $base_url . '$2$3';
  
  $absolute_html = 
    preg_replace("#(<\s*a\s+[^>]*href\s*=\s*[\"'])(?!http)([^\"'>]+)([\"'>]+)#", 
    $full_url, $relative_html);
  
  return $absolute_html;
}

/**
 * 
 * @param $dom - DOM of the XML file to search for elements
 * @param $elements_to_replace - the elements to match and replace
 */
function _replace_elements($html, $elements_to_replace) {
  $dom = new domDocument;
  $dom->loadHTML($html); //load the html into an object 
  $dom->preserveWhiteSpace = false; //discard white space
  
  $replace_key_pairs = explode(PHP_EOL, $elements_to_replace);

  foreach ($replace_key_pairs as $replace_key_pair) {

    $replace = explode('=', $replace_key_pair);
    
    if (isset($replace[0]) && isset($replace[1])) {
      $old_tag = trim($replace[0]);
      $new_tag = trim($replace[1]);
      $nodes = $dom->getElementsByTagName($old_tag);
      $toRemove = array();
      foreach ($nodes as $node) {
        $newNode = $dom->createElement($new_tag);
        foreach ($node->attributes as $attribute) {
          $newNode->setAttribute($attribute->name, $attribute->value);
        }

        foreach ($node->childNodes as $child) {
          $newNode->appendChild($node->removeChild($child));
        }

        $node->parentNode->appendChild($newNode);
        $toRemove[] = $node;
      }

      foreach ($toRemove as $node) {
        $node->parentNode->removeChild($node);
      }
    }
  }

  return $dom->saveXML($dom->documentElement);
}

/**
 * 
 * @param $dom - DOM of the XML file to search for elements
 * @param $elements_to_suppress - the elements to suppress
 */
function _suppress_elements(DOMNode $xpath_result, $elements_to_suppress) {
  $elements_array = explode(',', $elements_to_suppress);
  $child_nodes = $xpath_result->childNodes;
  
  foreach ($elements_array as $element_to_suppress) {  
    foreach($child_nodes as $child_node) {
      if ($child_node->nodeType == 1 && $child_node->nodeName == trim($element_to_suppress)) {
        $xpath_result->removeChild($child_node);
      }
    }
  }
 
  return $xpath_result;
}

/**
 * 
 * @param DOMNode $xpath_result
 * @param type $link_element
 * @return DOMNode
 */
function _add_href_to_first_element(DOMNode $xpath_result, $link_element) {
  $child_nodes = $xpath_result->childNodes;
  $link_found = false;
  
  foreach($child_nodes as $child_node) {
    if ($child_node->nodeType == 1 && $child_node->nodeName == trim($link_element)) {
      $link_found = $child_node->nodeValue;
    }
  }
  
  $index = NULL;
  
  //get the index of the first element
  if ($link_found) {
    $count = 0;
    $first_element_found = false;
    
    foreach($child_nodes as $child_node) {
      if ($child_node->nodeType == 1 && !$first_element_found) {
        $index = $count;
        $first_element_found = true;
      }   
      $count++;    
    }    
  }
  
  if($index){
    $xpath_result->childNodes->item($index)->setAttribute("href", (string)$link_found);
  }
  
  return $xpath_result;
}