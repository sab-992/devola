import { JobListing } from "./job-listing";

export interface Feed {
    last_updated_at: number;
    listings: JobListing[];
    website_name: string;
}