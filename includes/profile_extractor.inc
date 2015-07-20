<?php

/**
 * A Collection of functions that retrieve a profile
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
function _extract_profile($nid, $keywords) {
  $profile = NULL;
  
  $search_source_results_url = $GLOBALS['base_root'] . "/bento_search_query/" . $nid . "/" . $keywords;
  $search_source_results = file_get_contents($search_source_results_url);
 
  if(!$search_source_results) {
    return 'File Get Contents Failed';
  }
 
  // Retrieve the marker of the profile container
  $search_source = node_load($nid);
  $profile_container_field = $search_source->profile_container;
  $profile_container = $profile_container_field['und'][0]['value'];
  
  $valid_urls = _extract_valid_urls($search_source_results);
  
  foreach ($valid_urls as $valid_url) {
    $haystack = file_get_contents($valid_url);
    $profile = _extract_profile_using_marker($haystack, $profile_container);
    
    if($profile) {
      return $profile;
    }
  }
  
  return 'Not Found';
}

/**
 * Parses HTML and returns the contents of any element found with the specified class attribute.
 * 
 * @hay_stack the html to be parsed.
 * @needle the class to look for.
 * 
 */
function _extract_profile_using_marker($haystack, $needle) {
  $profile = NUll;
  $classname = substr($needle, 1);

  $doc = new DOMDocument();
  $doc->loadHTML($haystack);

  $xpath = new DOMXpath($doc);
  $nodes = $xpath->query("//*[@class='" . $classname . "']");
  
  if($nodes->length > 0) {
    foreach($nodes as $node) {
      return _innerHTML($node);
    }
  } 
  else {
    return NULL;  
  }
}

/**
 * Returns a string with the HTML content from a DOMDocument node element
 * 
 * @elm is the HTML element to be parsed.
 * 
 */
function _innerHTML(DOMNode $elm) { 
  $innerHTML = ''; 
  $children = $elm->childNodes;

  foreach($children as $child) { 
    $innerHTML .= $elm->ownerDocument->saveXML($child);
  }

  return $innerHTML;
}

/**
 * Parses HTML and returns an array with all valid urls.
 * 
 * @html is the HTML to be parsed.
 * 
 */
function _extract_valid_urls($html) {
  $valid_urls = array();
  
  $doc = new DOMDocument();
  $doc->loadHTML($html);

  $xpath = new DOMXpath($doc);
  $nodes = $xpath->query('//a');

  foreach($nodes as $node) {
    $valid_urls[] = $node->getAttribute('href');
  }
  
  return $valid_urls;
}