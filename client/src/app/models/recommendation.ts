export type RunStatus = 'pending' | 'running' | 'completed' | 'failed';


export interface RecommendationRun {
    uuid: string;
    started_at: number;
    last_updated_at: number;
    status: RunStatus;
}

export interface ResumeScore {
    tag: string;
    score: number;
}

export interface RecommendationResult {
    listing_id: number;
    title: string;
    company: string;
    link: string;
    website_host: string;
    website_endpoint: string;
    status: RunStatus;
    started_at: number;
    last_updated_at: number;
    scores: ResumeScore[];
}